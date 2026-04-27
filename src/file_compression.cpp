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

    while (output.data_size < decomp_size)
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

unsigned char* DecompressBuffer_Huffman(unsigned char* data, size_t size, size_t decomp_size, bool is8bit)
{
    unsigned char* buffer = new unsigned char[decomp_size];

    file_stream output = FileStreamBuild(buffer);
    file_stream stream = FileStreamBuild(data, size);

    uint8_t tree_size = FileStreamReadUint8(&stream);
    uint8_t tree_root = FileStreamPeekUint8(&stream);

    uint8_t* tree_buffer = stream.data + stream.cursor;
    FileStreamSetCursor(&stream, stream.cursor + size_t(tree_size * 2));

    uint8_t lsb_part = 0x80;
    uint8_t pos = tree_root;

    int i = 0, code = 0, next = 0;

    while (output.data_size < decomp_size)
    {
        if (i % 32 == 0)
            code = FileStreamReadUint32(&stream);

        next += ((pos & 0x3F) << 1) + 2;
        
        int dir = (code >> (31 - i)) % 2 == 0 ? 2 : 1;
        bool leaf = (pos >> 5 >> dir) % 2 != 0;

        pos = tree_buffer[next - dir];
        
        if (leaf)
        {
            if (is8bit)
            {
                FileStreamWriteUint8(&output, pos);
            }
            else
            {
                if ((lsb_part & 0x80) != 0)
                {
                    lsb_part = pos;
                }
                else
                {
                    FileStreamWriteUint8(&output, (pos << 4) | lsb_part);
                    lsb_part = 0x80;
                }
            }

            pos = tree_root;
            next = 0;
        }

        i++;
    }

    return buffer;
}

unsigned char* DecompressBuffer_Rle(unsigned char* data, size_t size, size_t decomp_size)
{
    unsigned char* buffer = new unsigned char[decomp_size];

    file_stream output = FileStreamBuild(buffer);
    file_stream stream = FileStreamBuild(data, size);

    while (output.data_size < decomp_size)
    {
        uint8_t flags = FileStreamReadUint8(&stream);
        if ((flags & 0x80) > 0)
        {
            uint8_t byte = FileStreamReadUint8(&stream);
            uint8_t reps = (flags & 0x7F) + 3;

            for (uint8_t i = 0; i < reps; i++)
                FileStreamWriteUint8(&output, byte);
        }
        else
        {
            uint8_t length = flags + 1;
            for (uint8_t i = 0; i < length; i++)
                FileStreamWriteUint8(&output, FileStreamReadUint8(&stream));
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
        case file_comp_type::HFM4:
        case file_comp_type::HFM8: {
            *decomp_size = (size_t(data[0]) >> 3 | size_t(data[1]) << 5 | size_t(data[2]) << 13 | size_t(data[3]) << 21);
            return DecompressBuffer_Huffman(data + 4, size, *decomp_size, type == file_comp_type::HFM8);
        }
        case file_comp_type::RLE: {
            *decomp_size = (size_t(data[0]) >> 3 | size_t(data[1]) << 5 | size_t(data[2]) << 13 | size_t(data[3]) << 21);
            return DecompressBuffer_Rle(data + 4, size, *decomp_size);
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
