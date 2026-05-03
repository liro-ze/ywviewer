#include "file_loader.h"

file* DispatchLoadFile(const char* path)
{
    const char* ext = GetFileExtension(path);

    file* file = nullptr;
    bool succ = false;

    if (strcmp(ext, ".xa") == 0)
    {
        file = new file_xa();
        succ = file->Load(path);
    }

    if (strcmp(ext, ".xc") == 0)
    {
        file = new file_xc();
        succ = file->Load(path);
    }

    if (strcmp(ext, ".xf") == 0)
    {
        file = new file_xf();
        succ = file->Load(path);
    }

    if (strcmp(ext, ".xi") == 0)
    {
        file = new file_xi();
        succ = file->Load(path);
    }

    if (strcmp(ext, ".xk") == 0)
    {
        file = new file_xk();
        succ = file->Load(path);
    }

    if (strcmp(ext, ".xr") == 0)
    {
        file = new file_xr();
        succ = file->Load(path);
    }

    if (file != nullptr && !succ)
    {
        delete file;
        file = nullptr;
    }

    return file;
}
