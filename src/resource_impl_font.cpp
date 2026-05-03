#include "res_def.h"

void res_font::Destroy()
{
	m_image.Destroy();

	m_charInfoList.clear();
	m_charSizeList.clear();
}
