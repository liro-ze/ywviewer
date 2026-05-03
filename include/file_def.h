#pragma once

#include "file_type.h"

class res_archive;
class res_model;
class res_image;
class res_image_anim;
class res_font;

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

    virtual bool LoadFromMemory(unsigned char* data, int size) { return false; }

    virtual bool BuildArchive(res_archive* archive) { return false; }

    virtual bool BuildModel(res_model* model) { return false; }

    virtual bool BuildImage(res_image* image) { return false; }

    virtual bool BuildImageAnim(res_image_anim* image_anim) { return false; }

    virtual bool BuildFont(res_font* font) { return false; }

    virtual void Unload() { }

protected:
    file_format m_format;
    file_type m_type;

    unsigned char* m_data;
    int m_size;
};

class file_xpck : public file
{
public:
    file_xpck();
    file_xpck(file_type type);

    static bool GetFileByName(unsigned char** output, int* output_size, unsigned char* data, int size, const char* targetName);

public:
    bool Load(const char* path) override;
    void Unload() override;

    bool BuildArchive(res_archive* archive) override;
};

class file_xc : public file_xpck
{
public:
    file_xc();

public:
    bool BuildModel(res_model* model) override;
};

class file_xf : public file_xpck
{
public:
    file_xf();

public:
    bool BuildFont(res_font* font) override;
};

class file_xi : public file
{
public:
    file_xi();

public:
    bool Load(const char* path) override;
    bool LoadFromMemory(unsigned char* data, int size) override;
    void Unload() override;

    bool BuildImage(res_image* image) override;
};

class file_xk : public file_xpck
{
public:
    file_xk();
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
    bool BuildImageAnim(res_image_anim* image_anim) override;
};
