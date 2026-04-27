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
        return m_fileData.size();
    }

    file_info* GetFileInfoByIdx(size_t idx)
    {
        return idx >= m_fileData.size() ? nullptr : &m_fileData[idx];
    }

    void Destroy();

private:
    std::vector<file_info> m_fileData;
};
