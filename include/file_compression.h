#pragma once

enum class file_comp_type : int
{
    NONE = 0, // No compression
    LZ10 = 1, // Lempel-Ziv Varient
    HFM4 = 2, // Huffman 4-bit
    HFM8 = 3, // Huffman 8-bit
    RTLE = 4, // Runtime Length Encoding
};

unsigned char* DecompressBuffer(unsigned char* data, size_t size, size_t* decomp_size);
