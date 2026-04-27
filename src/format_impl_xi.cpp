#include "file_def.h"
#include "res_def.h"

file_xi::file_xi()
    : file(file_format::IMGX, file_type::IMAGE)
{ }

bool file_xi::Load(const char* path)
{
    m_data = LoadFileData(path, &m_size);
    return m_data != nullptr && m_size > 0;
}

void file_xi::Unload()
{
    if (m_data != nullptr)
    {
        RL_FREE(m_data);

        m_data = nullptr;
        m_size = 0;
    }
}

bool file_xi::BuildImage(res_image* image)
{
    return false;
}
