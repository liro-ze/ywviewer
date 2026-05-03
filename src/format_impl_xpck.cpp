#include "file_def.h"
#include "res_def.h"
#include "file_stream.h"
#include "file_compression.h"
#include "crc32b.h"

file_xpck::file_xpck()
    : file(file_format::XPCK, file_type::ARCHIVE)
{ }

file_xpck::file_xpck(file_type type)
    : file(file_format::XPCK, type | file_type::ARCHIVE)
{ }

bool file_xpck::Load(const char* path)
{
    m_data = LoadFileData(path, &m_size);
    return m_data != nullptr && m_size > 0;
}

void file_xpck::Unload()
{
    if (m_data != nullptr)
    {
        RL_FREE(m_data);

        m_data = nullptr;
        m_size = 0;
    }
}

bool file_xpck::GetFileByName(unsigned char** output, int* output_size, unsigned char* data, int size, const char* targetName)
{
    if (output == nullptr || output_size == nullptr || data == nullptr || size <= 0 || targetName == nullptr)
        return false;

    bool succ = false;

    file_stream stream = FileStreamBuild(data, size);
    if (FileStreamReadUint32(&stream) != 0)
    {
        uint16_t fileCountAndFlags = FileStreamReadUint16(&stream);

        uint16_t fileCount = fileCountAndFlags & 0xFFF;
        uint8_t fileFlags = fileCountAndFlags >> 12;

        uint16_t fileInfoOffset = FileStreamReadUint16(&stream) << 2;
        uint16_t fileNameOffset = FileStreamReadUint16(&stream) << 2;
        uint16_t fileDataOffset = FileStreamReadUint16(&stream) << 2;

        uint16_t fileInfoSize = FileStreamReadUint16(&stream) << 2;
        uint16_t fileNameSize = FileStreamReadUint16(&stream) << 2;
        uint32_t fileDataSize = FileStreamReadUint32(&stream) << 2;

        uint32_t targetNameHash = crc32b_compute(targetName);

        FileStreamSetCursor(&stream, fileInfoOffset);

        uint16_t i = 0;
        bool found = false;

        while (i < fileCount && !found)
        {
            uint32_t fileHash = FileStreamReadUint32(&stream);
            uint16_t nameOffset = FileStreamReadUint16(&stream);

            uint16_t lsbDataOffset = FileStreamReadUint16(&stream);
            uint16_t lsbDataSize = FileStreamReadUint16(&stream);

            uint8_t msbDataOffset = FileStreamReadUint8(&stream);
            uint8_t msbDataSize = FileStreamReadUint8(&stream);

            uint32_t dataOffset = ((msbDataOffset << 16) | lsbDataOffset) << 2;
            uint32_t dataSize = (msbDataSize << 16) | lsbDataSize;

            if (targetNameHash == fileHash)
            {
                *output = (data + fileDataOffset + dataOffset);
                *output_size = dataSize;
                
                found = true;
            }

            i++;
        }

        succ = found;
    }

    return succ;
}

bool file_xpck::BuildArchive(res_archive* archive)
{
    if (m_data == nullptr || m_size <= 0)
        return false;

    bool succ = false;

    file_stream stream = FileStreamBuild(m_data, m_size);
    if (FileStreamReadUint32(&stream) != 0)
    {
        uint16_t fileCountAndFlags = FileStreamReadUint16(&stream);
        
        uint16_t fileCount = fileCountAndFlags & 0xFFF;
        uint8_t fileFlags = fileCountAndFlags >> 12;

        uint16_t fileInfoOffset = FileStreamReadUint16(&stream) << 2;
        uint16_t fileNameOffset = FileStreamReadUint16(&stream) << 2;
        uint16_t fileDataOffset = FileStreamReadUint16(&stream) << 2;

        uint16_t fileInfoSize = FileStreamReadUint16(&stream) << 2;
        uint16_t fileNameSize = FileStreamReadUint16(&stream) << 2;
        uint32_t fileDataSize = FileStreamReadUint32(&stream) << 2;

        FileStreamSetCursor(&stream, fileNameOffset);

        size_t decompNameSize;
        unsigned char* decompNameData = DecompressBuffer(
            FileStreamGetAtCursor(&stream), fileNameSize, &decompNameSize
        );

        if (decompNameData == nullptr)
            goto end;

        FileStreamSetCursor(&stream, fileInfoOffset);

        for (uint16_t i = 0; i < fileCount; i++)
        {
            uint32_t fileHash = FileStreamReadUint32(&stream);
            uint16_t nameOffset = FileStreamReadUint16(&stream);
            
            uint16_t lsbDataOffset = FileStreamReadUint16(&stream);
            uint16_t lsbDataSize = FileStreamReadUint16(&stream);

            uint8_t msbDataOffset = FileStreamReadUint8(&stream);
            uint8_t msbDataSize = FileStreamReadUint8(&stream);

            uint32_t dataOffset = ((msbDataOffset << 16) | lsbDataOffset) << 2;
            uint32_t dataSize = (msbDataSize << 16) | lsbDataSize;

            const char* strName = (const char*)(decompNameData + nameOffset);

            res_archive::file_info info = {};
            info.fileName = std::string(strName, strlen(strName));
            info.dataSize = dataSize;

            archive->m_filelist.push_back(info);
        }

        delete decompNameData;

        succ = true;
    }

end:
    return succ;
}
