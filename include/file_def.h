#pragma once

#include <vector>
#include <string>

#include "file_type.h"

class file_archive;
class file_model;
class file_image;
class file_image_anim;
class file_font;

class file
{
public:
    file(file_format fmt, file_type type)
        : m_format(fmt), m_type(type)
        , m_data(nullptr), m_size(0)
    { }

public:
    file_format GetFormat() const { return m_format; }

    file_type GetType() const { return m_type; }

    virtual bool Load(const char* path) { return false; }

    virtual bool BuildArchive(file_archive* archive) { return false; }

    virtual bool BuildModel(file_model* model) { return false; }

    virtual bool BuildImage(file_image* image) { return false; }

    virtual bool BuildImageAnim(file_image_anim* image_anim) { return false; }

    virtual bool BuildFont(file_font* font) { return false; }

    virtual void Unload() { }

protected:
    file_format m_format;
    file_type m_type;

    unsigned char* m_data;
    int m_size;
};

class file_archive
{
    friend class file_xpck;

public:
    struct file_archive_info
    {
        std::string fileName;
        size_t dataSize;
    };

public:
    file_archive()
    { }

    size_t GetFileCount() const
    {
        return m_fileData.size();
    }

    file_archive_info* GetFileInfoByIdx(size_t idx)
    {
        return idx >= m_fileData.size()  ? nullptr : &m_fileData[idx];
    }

private:
    std::vector<file_archive_info> m_fileData;
};

class file_xpck : public file
{
public:
    file_xpck();
    file_xpck(file_type type);

public:
    bool Load(const char* path) override;
    void Unload() override;

    bool BuildArchive(file_archive* archive) override;
};

class file_xc : public file_xpck
{
public:
    file_xc();

public:
    bool BuildModel(file_model* model) override;
};

class file_xr : public file_xpck
{
public:
    file_xr();
};

class file_xa : public file_xpck
{
public:
    file_xa();

public:
    bool BuildImageAnim(file_image_anim* image_anim) override;
};
