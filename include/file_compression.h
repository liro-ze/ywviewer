#pragma once

enum class file_comp_type : int
{
    NONE = 0, // No compression
    LZ10 = 1, // Lempel-Ziv Variant
    HFM4 = 2, // Huffman 4-bit
    HFM8 = 3, // Huffman 8-bit
    RLE = 4, // Run-Length Encoding
};

unsigned char* DecompressBuffer(unsigned char* data, size_t size, size_t* decomp_size);
