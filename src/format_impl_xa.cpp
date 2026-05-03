#include "file_def.h"
#include "res_def.h"

file_xa::file_xa()
	: file_xpck(file_type::IMAGE_ANIM)
{ }

bool file_xa::BuildImageAnim(res_image_anim* image_anim)
{
	if (m_data == nullptr || m_size <= 0)
		return false;

	bool succ = false;
	
	unsigned char* binData;
	int binDataSize;
	if (!this->GetFileByName(&binData, &binDataSize, m_data, m_size, "RES.bin"))
		goto end;

end:
	return succ;
}
