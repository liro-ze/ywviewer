#include "file_def.h"
#include "res_def.h"

file_xc::file_xc()
    : file_xpck(file_type::MODEL)
{ }


bool file_xc::BuildModel(res_model* model)
{
	return false;
}
