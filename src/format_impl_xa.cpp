#include "file_def.h"
#include "res_def.h"

file_xa::file_xa()
	: file_xpck(file_type::IMAGE_ANIM)
{ }

bool file_xa::BuildImageAnim(res_image_anim* image_anim)
{
	return false;
}
