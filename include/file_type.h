#pragma once

#include <cstring>
#include <raylib.h>

enum class file_type : int
{
    UNKNOWN    = 0,
    ARCHIVE    = 2,
    MODEL      = 4,
    IMAGE      = 8,
    FONT       = 16,
    IMAGE_ANIM = 32
};

enum class file_format : int
{
    UNKNOWN = 0,

    /* Archives */
    XPCK,
    XFNT,
    XANM,
    
    IMGX,
};

inline file_type operator |(file_type a, file_type b)
{
    return static_cast<file_type>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool IsFileArchive(const char* path)
{
    const char* path_ext = GetFileExtension(path);
    return strcmp(path_ext, ".xa") == 0 ||
           strcmp(path_ext, ".xc") == 0 ||
           strcmp(path_ext, ".xf") == 0 ||
           strcmp(path_ext, ".xk") == 0 ||
           strcmp(path_ext, ".xr") == 0 ||
           strcmp(path_ext, ".xv") == 0;
}

inline bool IsFileModel(const char* path)
{
    const char* path_ext = GetFileExtension(path);
    return strcmp(path_ext, ".xc") == 0;
}

inline bool IsFileImage(const char* path)
{
    const char* path_ext = GetFileExtension(path);
    return strcmp(path_ext, ".xi") == 0;
}

inline bool IsFileImageAnimated(const char* path)
{
    const char* path_ext = GetFileExtension(path);
    return strcmp(path_ext, ".xa") == 0;
}

inline bool IsFileFont(const char* path)
{
    const char* path_ext = GetFileExtension(path);
    return strcmp(path_ext, ".xf") == 0;
}
