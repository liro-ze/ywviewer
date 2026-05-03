#include "res_def.h"
#include <raylib.h>
#include <math.h>
#include "file_stream.h"

uint8_t ETC1ZOrderCount = 16;
uint8_t ETC1ZOrder[16] = { 0, 4, 1, 5, 8, 12, 9, 13, 2, 6, 3, 7, 10, 14, 11, 15 };

int ETC1Modifiers[8][4] = {
	{ 2, 8, -2, -8 }, { 5, 17, -5, -17 },
	{ 9, 29, -9, -29 }, { 13, 42, -13, -42 },
	{ 18, 60, -18, -60 }, { 24, 80, -24, -80 },
	{ 33, 106, -33, -106 }, { 47, 183, -47, -183 }
};

uint8_t SwizzleBitFieldCount = 6;
uint8_t SwizzleBitFields[6][2] = { { 0, 1 }, { 1, 0 }, { 0, 2 }, { 2, 0 }, { 0, 4 }, { 4, 0 } };


void res_image::ReverseChannels(res_image::Image* img)
{
	int stride = this->m_bitDepth / 8;
	
	for (size_t i = 0; i < img->dataSize; i += stride)
	{
		uint8_t color[4] = { 0 };
		for (int j = 0; j < stride; j++)
			color[j] = img->data[i + j];

		for (int j = 0; j < stride; j++)
			img->data[i + j] = color[stride - j - 1];
	}
}

void res_image::DecodeETC1(res_image::Image* img, bool useAlpha)
{
	int imgBlockCount = m_width * m_height / 16;

	file_stream stream = FileStreamBuild(img->data, img->dataSize);

	img->dataSize = (m_width * m_height * this->m_bitDepth / 8);
	unsigned char* data = new unsigned char[img->dataSize];

	file_stream output = FileStreamBuild(data);
	
	for (int i = 0; i < imgBlockCount; i++)
	{
		uint64_t alpha = (useAlpha ? FileStreamReadUint64(&stream) : UINT64_MAX);
		uint64_t block = FileStreamReadUint64(&stream);

		uint16_t lsb = (block & 0xFFFF);
		uint16_t msb = (block >> 16) & 0xFFFF;
		uint16_t flags = (block >> 32) & 0xFF;

		uint8_t b = (block >> 40) & 0xFF;
		uint8_t g = (block >> 48) & 0xFF;
		uint8_t r = (block >> 56) & 0xFF;
		
		bool flipBit = (flags & 1) == 1;
		bool diffBit = (flags & 2) == 2;

		int colorDepth = diffBit ? 32 : 16;

		int table0 = flags >> 5 & 7;
		int table1 = flags >> 2 & 7;

		uint8_t color0[3] = { r * colorDepth / 256, g * colorDepth / 256, b * colorDepth / 256 };
		uint8_t color1[3] = { r % 16, g % 16, b % 16 };
		
		if (diffBit)
		{
			color1[0] = color0[0] + (((r % 8) + 4) % 8 - 4);
			color1[1] = color0[1] + (((g % 8) + 4) % 8 - 4);
			color1[2] = color0[2] + (((b % 8) + 4) % 8 - 4);
		}

		if (colorDepth == 16)
		{
			color0[0] = color0[0] * 17;
			color0[1] = color0[1] * 17;
			color0[2] = color0[2] * 17;

			color1[0] = color1[0] * 17;
			color1[1] = color1[1] * 17;
			color1[2] = color1[2] * 17;
		}
		else
		{
			color0[0] = color0[0] << 3 | color0[0] >> 2;
			color0[1] = color0[1] << 3 | color0[1] >> 2;
			color0[2] = color0[2] << 3 | color0[2] >> 2;

			color1[0] = color1[0] << 3 | color1[0] >> 2;
			color1[1] = color1[1] << 3 | color1[1] >> 2;
			color1[2] = color1[2] << 3 | color1[2] >> 2;
		}

		int flipBitMask = flipBit ? 2 : 8;

		for (int i = 0; i < ETC1ZOrderCount; i++)
		{
			int zIdx = ETC1ZOrder[i];

			int mod = ETC1Modifiers[(zIdx & flipBitMask) == 0 ? table0 : table1][(msb >> zIdx) % 2 * 2 + (lsb >> zIdx) % 2];

			uint8_t color[3] = { 0 };
			if ((zIdx & flipBitMask) == 0)
			{
				color[0] = std::max(0, std::min(color0[0] + mod, 255));
				color[1] = std::max(0, std::min(color0[1] + mod, 255));
				color[2] = std::max(0, std::min(color0[2] + mod, 255));
			}
			else
			{
				color[0] = std::max(0, std::min(color1[0] + mod, 255));
				color[1] = std::max(0, std::min(color1[1] + mod, 255));
				color[2] = std::max(0, std::min(color1[2] + mod, 255));
			}

			FileStreamWriteUint8(&output, color[0]);
			FileStreamWriteUint8(&output, color[1]);
			FileStreamWriteUint8(&output, color[2]);
			FileStreamWriteUint8(&output, (alpha >> 4 * zIdx) % 16 * 17);
		}
	}

	delete img->data;
	img->data = data;
}

void res_image::Decode(res_image::Image* img)
{
	if (m_format == 0x1B || m_format == 0x1C)
		this->m_bitDepth = 32; // Convert ETC1 to RGBA

	switch (m_format)
	{
		case 0x0:
		case 0x1:
		//case 0x2:
		case 0x3:
		//case 0x4:
			ReverseChannels(img);
			break;
		case 0x1B:
			DecodeETC1(img, false);
			break;
		case 0x1C:
			DecodeETC1(img, true);
			break;
	}
}

void res_image::SwizzleBuild(res_image::Image* img)
{
	int MacroTileWidth = 0;
	int MacroTileHeight = 0;

	for (int i = 0; i < SwizzleBitFieldCount; i++)
	{
		MacroTileWidth |= SwizzleBitFields[i][0];
		MacroTileHeight |= SwizzleBitFields[i][1];
	}

	MacroTileWidth += 1;
	MacroTileHeight += 1;

	int imageStride = (m_width + 7) & ~7;
	int widthInTiles = (imageStride + MacroTileWidth - 1) / MacroTileWidth;

	int stride = (m_bitDepth / 8);

	unsigned char* data = new unsigned char[img->dataSize];
	file_stream output = FileStreamBuild(data);

	int pixelCount = m_width * m_height;
	for (int i = 0; i < pixelCount; i++)
	{
		int macroTileCount = int(i) / (MacroTileWidth * MacroTileHeight);

		int macroX = macroTileCount % widthInTiles;
		int macroY = macroTileCount / widthInTiles;

		int x = macroX * MacroTileWidth;
		int y = macroY * MacroTileHeight;

		for (int j = 0; j < SwizzleBitFieldCount; j++)
		{
			int bit = (i >> j) & 1;

			x ^= bit * SwizzleBitFields[j][0];
			y ^= bit * SwizzleBitFields[j][1];
		}

		/*for (int j = 0; j < stride; j++)
			data[(y * m_width + x) * stride + j] = img->data[i * stride + (4 - j - 1)];*/

		//data[(y * m_width + x) * stride + 0] = img->data[i * stride + 3];
		//data[(y * m_width + x) * stride + 1] = img->data[i * stride + 2];
		//data[(y * m_width + x) * stride + 2] = img->data[i * stride + 1];
		//data[(y * m_width + x) * stride + 3] = img->data[i * stride + 0];

		FileStreamSetCursor(&output, size_t((y * m_width + x) * stride));

		for (int j = 0; j < stride; j++)
			FileStreamWriteUint8(&output, img->data[i * stride + j]);
	}

	delete img->data;
	img->data = data;
}

void res_image::CreateTexture(res_image::Image* img)
{
	PixelFormat format = static_cast<PixelFormat>(0);
	switch (m_format)
	{
		case 0x00: format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8; break;
		case 0x01: format = PIXELFORMAT_UNCOMPRESSED_R4G4B4A4; break;
		case 0x02: format = PIXELFORMAT_UNCOMPRESSED_R5G5B5A1; break;
		case 0x03: format = PIXELFORMAT_UNCOMPRESSED_R8G8B8; break; // BGR888: swap channels manually
		case 0x04: format = PIXELFORMAT_UNCOMPRESSED_R5G6B5; break;
		//case 0x0A: format = 0; break;
		//case 0x0B: format = 0; break;
		//case 0x0C: format = 0; break;
		//case 0x0D: format = 0; break;
		//case 0x0E: format = 0; break;
		//case 0x0F: format = 0; break;
		case 0x1B: format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8; break; // ETC1: convert to RGBA
		case 0x1C: format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8; break; // ETC1A4: convert to RGBA
	}

	img->tex = LoadTextureFromImage({
		img->data, m_width, m_height, 1, format
	});
}

void res_image::Destroy()
{
	for (size_t i = 0; i < m_images.size(); i++)
	{
		if (m_images[i].tex.id != 0)
			UnloadTexture(m_images[i].tex);

		delete m_images[i].data;
	}
}
