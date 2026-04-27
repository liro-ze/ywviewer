#pragma once

#include <cstdint>

struct file_stream
{
	unsigned char* data;
	size_t data_size;
	size_t cursor;
};

file_stream FileStreamBuild(unsigned char* data, size_t size);
file_stream FileStreamBuild(unsigned char* data);

unsigned char* FileStreamGetAtCursor(file_stream* stream);
void FileStreamSetCursor(file_stream* stream, size_t offset);

uint8_t FileStreamPeekUint8(file_stream* stream);
uint16_t FileStreamPeekUint16(file_stream* stream);
uint32_t FileStreamPeekUint32(file_stream* stream);
uint64_t FileStreamPeekUint64(file_stream* stream);

uint8_t FileStreamReadUint8(file_stream* stream);
uint16_t FileStreamReadUint16(file_stream* stream);
uint32_t FileStreamReadUint32(file_stream* stream);
uint64_t FileStreamReadUint64(file_stream* stream);

void FileStreamWriteUint8(file_stream* stream, uint8_t value);
void FileStreamWriteUint16(file_stream* stream, uint16_t value);
void FileStreamWriteUint32(file_stream* stream, uint32_t value);
void FileStreamWriteUint64(file_stream* stream, uint64_t value);
