#pragma once

#include <vector>
#include <string>

class res_archive
{
    friend class file_xpck;

public:
    struct file_info
    {
        std::string fileName;
        size_t dataSize;
    };

public:
    size_t GetFileCount() const
    {
        return m_filelist.size();
    }

    file_info* GetFileInfoByIdx(size_t idx)
    {
        return idx >= m_filelist.size() ? nullptr : &m_filelist[idx];
    }

    void Destroy();

private:
    std::vector<file_info> m_filelist;
};

class res_image
{
    friend class file_xi;

public:

private:
    int m_imageCount;
    int m_width;
    int m_height;
    int m_format;
    int m_bitDepth;
};