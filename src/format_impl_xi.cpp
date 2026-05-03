#include "file_def.h"
#include "res_def.h"
#include "file_stream.h"
#include "file_compression.h"

file_xi::file_xi()
    : file(file_format::IMGX, file_type::IMAGE)
{ }

bool file_xi::Load(const char* path)
{
    m_data = LoadFileData(path, &m_size);
    return m_data != nullptr && m_size > 0;
}

bool file_xi::LoadFromMemory(unsigned char* data, int size)
{
    m_data = data;
    m_size = size;

    return data != nullptr && size > 0;
}

void file_xi::Unload()
{
    if (m_data != nullptr)
    {
        RL_FREE(m_data);

        m_data = nullptr;
        m_size = 0;
    }
}

bool file_xi::BuildImage(res_image* image)
{
    if (m_data == nullptr || m_size <= 0)
        return false;

    bool succ = false;

    file_stream stream = FileStreamBuild(m_data, m_size);
    if (FileStreamReadUint64(&stream) != 0)
    {
        uint16_t entryOffset = FileStreamReadUint16(&stream);

        uint8_t imgFormat = FileStreamReadUint8(&stream);
        uint8_t imgConst0 = FileStreamReadUint8(&stream);
        uint8_t imgCount = FileStreamReadUint8(&stream);
        uint8_t bitDepth = FileStreamReadUint8(&stream);

        uint16_t tileByteDepth = FileStreamReadUint16(&stream);
        uint16_t imgWidth = FileStreamReadUint16(&stream);
        uint16_t imgHeight = FileStreamReadUint16(&stream);

        image->m_width = imgWidth;
        image->m_height = imgHeight;
        image->m_format = imgFormat;
        image->m_bitDepth = bitDepth;

        uint16_t paletteTableOffset = FileStreamReadUint16(&stream);
        uint16_t paletteTableCount = FileStreamReadUint16(&stream);

        uint16_t imageTableOffset = FileStreamReadUint16(&stream);
        uint16_t imageTableCount = FileStreamReadUint16(&stream);

        uint32_t globalDataOffset = FileStreamReadUint32(&stream);

        for (uint16_t i = 0; i < paletteTableCount; i++)
        {
            FileStreamSetCursor(&stream, size_t(paletteTableOffset + i * 16));

            uint32_t dataOffset = FileStreamReadUint32(&stream);
            uint32_t dataSize = FileStreamReadUint32(&stream);

            uint16_t colorCount = FileStreamReadUint16(&stream);

            uint8_t const0 = FileStreamReadUint8(&stream);
            uint8_t format = FileStreamReadUint8(&stream);

            // padding
            FileStreamReadUint32(&stream);

            FileStreamSetCursor(&stream, size_t(globalDataOffset + dataOffset));

            size_t decompPaletteDataSize;
            uint8_t* decompPaletteData = DecompressBuffer(
                FileStreamGetAtCursor(&stream), dataSize, &decompPaletteDataSize
            );

            if (decompPaletteData == nullptr)
                goto end;

            // add to image if needed
        }

        for (uint16_t i = 0; i < imageTableCount; i++)
        {
            FileStreamSetCursor(&stream, size_t(imageTableOffset + i * 24));

            uint32_t tileDataOffset = FileStreamReadUint32(&stream);
            uint32_t tileDataSize = FileStreamReadUint32(&stream);

            uint32_t imageDataOffset = FileStreamReadUint32(&stream);
            uint32_t imageDataSize = FileStreamReadUint32(&stream);

            // padding
            FileStreamReadUint64(&stream);

            FileStreamSetCursor(&stream, size_t(globalDataOffset + tileDataOffset));

            size_t decompTileDataSize;
            uint8_t* decompTileData = DecompressBuffer(
                FileStreamGetAtCursor(&stream), tileDataSize, &decompTileDataSize
            );

            if (decompTileData == nullptr)
                goto end;

            FileStreamSetCursor(&stream, size_t(globalDataOffset + imageDataOffset));

            size_t decompImageDataSize;
            uint8_t* decompImageData = DecompressBuffer(
                FileStreamGetAtCursor(&stream), imageDataSize, &decompImageDataSize
            );

            if (decompImageData == nullptr)
                goto end;

            file_stream tileStream = FileStreamBuild(decompTileData, decompTileDataSize);
            file_stream imageStream = FileStreamBuild(decompImageData, decompImageDataSize);

            size_t entryCount = decompTileDataSize / 2;
            bool useLargeEntry = false;

            uint16_t legacyIndicator = FileStreamPeekUint16(&tileStream);
            if (legacyIndicator == 0x453)
            {
                uint64_t legacyHeader = FileStreamReadUint64(&tileStream);

                entryCount = (decompTileDataSize - 8) / 4;
                useLargeEntry = true;
            }

            unsigned char* data = new unsigned char[entryCount * tileByteDepth];

            for (size_t j = 0; j < entryCount; j++)
            {
                uint32_t entry = (useLargeEntry ? FileStreamReadUint32(&tileStream) : FileStreamReadUint16(&tileStream));

                FileStreamSetCursor(&imageStream, size_t(entry * tileByteDepth));

                for (uint16_t k = 0; k < tileByteDepth; k++)
                    data[j * tileByteDepth + k] = FileStreamReadUint8(&imageStream);
            }

            int targetWidth = (imgWidth + 7) & ~7;
            int targetHeight = (imgHeight + 7) & ~7;

            for (size_t j = 0; j < 1; j++)
            {
                res_image::Image img = { 0 };
                img.data = data;
                img.dataSize = size_t(targetWidth * targetHeight * image->m_bitDepth / 8);

                image->Decode(&img);
                image->SwizzleBuild(&img);

                image->m_images.push_back(img);
            }

            delete decompTileData;
            delete decompImageData;

            succ = true;
        }
    }

end:
    return succ;
}
