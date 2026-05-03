#pragma once

#include <vector>
#include <string>
#include <raylib.h>

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
    struct Image
    {
        unsigned char* data;
        size_t dataSize;
        Texture2D tex;
    };

public:
    std::vector<res_image::Image>& GetImages()
    {
        return m_images;
    }

    int GetWidth() const
    {
        return m_width;
    }

    int GetHeight() const
    {
        return m_height;
    }
    
    void ReverseChannels(res_image::Image* img);

    void DecodeETC1(res_image::Image* img, bool alpha);
    void Decode(res_image::Image* img);

    void SwizzleBuild(res_image::Image* img);
    void CreateTexture(res_image::Image* img);

    void Destroy();

private:
    std::vector<res_image::Image> m_images;

    int m_imageCount;
    int m_bitDepth;
    int m_format;
    int m_width;
    int m_height;
};

class res_image_anim
{
    friend class file_xa;

public:
    void Destroy();

private:
    std::vector<res_image> m_images;
};

class res_font
{
    friend class file_xf;

public:
    struct CharSizeInfo
    {
        uint8_t offsetX;
        uint8_t offsetY;
        uint8_t glyphWidth;
        uint8_t glyphHeight;
    };

    struct CharInfo
    {
        uint16_t code;
        uint16_t width;

        uint16_t imgOffsetX;
        uint16_t imgOffsetY;
        uint16_t imgIndex;

        CharSizeInfo* bounds;
        bool is_small;
    };

public:
    size_t GetGlyphCount() const
    {
        return m_charInfoList.size();
    }

    uint16_t GetLargeCharHeight() const
    {
        return largeCharHeight;
    }

    uint16_t GetSmallCharHeight() const
    {
        return smallCharHeight;
    }

    res_font::CharInfo* GetGlyphByCharCode(uint16_t code)
    {
        for (size_t i = 0; i < m_charInfoList.size(); i++)
        {
            if (m_charInfoList[i].code == code)
                return &m_charInfoList[i];
        }

        return nullptr;
    }

    res_image* GetImage()
    {
        return &m_image;
    }

    void Destroy();

private:
    uint16_t defaultLargeChar;
    uint16_t defaultSmallChar;
    uint16_t largeCharHeight;
    uint16_t smallCharHeight;

    std::vector<res_font::CharSizeInfo> m_charSizeList;
    std::vector<res_font::CharInfo> m_charInfoList;
    res_image m_image;
};
