#include "file_compression.h"
#include "file_stream.h"

#include <memory.h>

unsigned char* DecompressBuffer_None(unsigned char* data, size_t decomp_size)
{
    unsigned char* buffer = new unsigned char[decomp_size];
    memcpy(buffer, data, decomp_size);
    return buffer;
}

unsigned char* DecompressBuffer_Lz10(unsigned char* data, size_t size, size_t decomp_size)
{
    unsigned char* buffer = new unsigned char[decomp_size];

    file_stream output = FileStreamBuild(buffer);
    file_stream stream = FileStreamBuild(data, size);

    int flags = 0, mask = 1;

    while (stream.cursor < stream.data_size && output.data_size < decomp_size)
    {
        if (mask == 1)
        {
            flags = FileStreamReadUint8(&stream);
            mask = 0x80;
        }
        else
        {
            mask >>= 1;
        }

        if ((flags & mask) > 0)
        {
            uint8_t byte1 = FileStreamReadUint8(&stream);
            uint8_t byte2 = FileStreamReadUint8(&stream);

            uint16_t length = (byte1 >> 4) + 3;
            uint16_t displacement = (((byte1 & 0x0F) << 8) | byte2) + 1;

            for (uint16_t i = 0; i < length; i++)
            {
                FileStreamWriteUint8(&output, output.data[output.data_size - displacement]);
            }
        }
        else
        {
            uint8_t byte = FileStreamReadUint8(&stream);
            FileStreamWriteUint8(&output, byte);
        }
    }

    return buffer;
}

unsigned char* DecompressBuffer(unsigned char* data, size_t size, size_t* decomp_size)
{
    /*
    
    In compliance with Yo-kai Watch Engine, the default method is using no compression
    
    */

    file_comp_type type = static_cast<file_comp_type>(data[0] & 0x7);
    switch (type)
    {
        case file_comp_type::LZ10: {
            *decomp_size = (size_t(data[0]) >> 3 | size_t(data[1]) << 5 | size_t(data[2]) << 13 | size_t(data[3]) << 21);
            return DecompressBuffer_Lz10(data + 4, size, *decomp_size);
        }
        default: {
            *decomp_size = (size - 4);
            return DecompressBuffer_None(data + 4, *decomp_size);
            break;
        }
    }

    *decomp_size = 0;
    return nullptr;
}
