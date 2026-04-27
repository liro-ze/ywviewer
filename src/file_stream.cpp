#include "file_stream.h"

file_stream FileStreamBuild(unsigned char* data, size_t size)
{
    file_stream stream = { 0 };
    stream.data = data;
    stream.data_size = size;
    stream.cursor = 0;

    return stream;
}

file_stream FileStreamBuild(unsigned char* data)
{
    file_stream stream = { 0 };
    stream.data = data;
    stream.data_size = 0;
    stream.cursor = 0;

    return stream;
}

unsigned char* FileStreamGetAtCursor(file_stream* stream)
{
    return stream->data + stream->cursor;
}

void FileStreamSetCursor(file_stream* stream, size_t offset)
{
    stream->cursor = offset;
}

uint8_t FileStreamPeekUint8(file_stream* stream)
{
    if (stream->cursor + sizeof(uint8_t) > stream->data_size)
        return 0;

    return *((uint8_t*)(stream->data + stream->cursor));
}

uint16_t FileStreamPeekUint16(file_stream* stream)
{
    if (stream->cursor + sizeof(uint16_t) > stream->data_size)
        return 0;

    return *((uint16_t*)(stream->data + stream->cursor));
}

uint32_t FileStreamPeekUint32(file_stream* stream)
{
    if (stream->cursor + sizeof(uint32_t) > stream->data_size)
        return 0;

    return *((uint32_t*)(stream->data + stream->cursor));
}

uint64_t FileStreamPeekUint64(file_stream* stream)
{
    if (stream->cursor + sizeof(uint64_t) > stream->data_size)
        return 0;

    return *((uint64_t*)(stream->data + stream->cursor));
}

uint8_t FileStreamReadUint8(file_stream* stream)
{
    if (stream->cursor + sizeof(uint8_t) > stream->data_size)
        return 0;

    uint8_t value = *((uint8_t*)(stream->data + stream->cursor));
    stream->cursor += sizeof(uint8_t);

    return value;
}

uint16_t FileStreamReadUint16(file_stream* stream)
{
    if (stream->cursor + sizeof(uint16_t) > stream->data_size)
        return 0;

    uint16_t value = *((uint16_t*)(stream->data + stream->cursor));
    stream->cursor += sizeof(uint16_t);

    return value;
}

uint32_t FileStreamReadUint32(file_stream* stream)
{
    if (stream->cursor + sizeof(uint32_t) > stream->data_size)
        return 0;

    uint32_t value = *((uint32_t*)(stream->data + stream->cursor));
    stream->cursor += sizeof(uint32_t);

    return value;
}

uint64_t FileStreamReadUint64(file_stream* stream)
{
    if (stream->cursor + sizeof(uint64_t) > stream->data_size)
        return 0;

    uint64_t value = *((uint64_t*)(stream->data + stream->cursor));
    stream->cursor += sizeof(uint64_t);

    return value;
}

void FileStreamWriteUint8(file_stream* stream, uint8_t value)
{
    *((uint8_t*)(stream->data + stream->cursor)) = value;

    stream->data_size += sizeof(uint8_t);
    stream->cursor += sizeof(uint8_t);
}

void FileStreamWriteUint16(file_stream* stream, uint16_t value)
{
    *((uint16_t*)(stream->data + stream->cursor)) = value;

    stream->data_size += sizeof(uint16_t);
    stream->cursor += sizeof(uint16_t);
}

void FileStreamWriteUint32(file_stream* stream, uint32_t value)
{
    *((uint32_t*)(stream->data + stream->cursor)) = value;

    stream->data_size += sizeof(uint32_t);
    stream->cursor += sizeof(uint32_t);
}

void FileStreamWriteUint64(file_stream* stream, uint64_t value)
{
    *((uint64_t*)(stream->data + stream->cursor)) = value;

    stream->data_size += sizeof(uint64_t);
    stream->cursor += sizeof(uint64_t);
}
