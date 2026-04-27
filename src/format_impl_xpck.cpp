#include "file_def.h"
#include "res_def.h"
#include "file_stream.h"
#include "file_compression.h"

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
            
            uint16_t msbDataOffset = FileStreamReadUint16(&stream);
            uint16_t msbDataSize = FileStreamReadUint16(&stream);

            uint8_t lsbDataOffset = FileStreamReadUint8(&stream);
            uint8_t lsbDataSize = FileStreamReadUint8(&stream);

            uint32_t dataOffset = (msbDataOffset << 8) | lsbDataOffset;
            uint32_t dataSize = (msbDataSize << 8) | lsbDataSize;

            const char* strName = (const char*)(decompNameData + nameOffset);

            res_archive::file_info info = {};
            info.fileName = std::string(strName, strlen(strName));
            info.dataSize = dataSize;

            archive->m_fileData.push_back(info);
        }

        delete decompNameData;

        succ = true;
    }

end:
    return succ;
}
