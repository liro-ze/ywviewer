#include "file_def.h"
#include "res_def.h"
#include "file_stream.h"
#include "file_compression.h"

file_xf::file_xf()
	: file_xpck(file_type::FONT)
{ }

bool file_xf::BuildFont(res_font* font)
{
	if (m_data == nullptr || m_size <= 0)
		return false;

	bool succ = false;

	unsigned char* imgData;
	int imgDataSize;
	if (!this->GetFileByName(&imgData, &imgDataSize, m_data, m_size, "000.xi"))
		goto end;

	unsigned char* binData;
	int binDataSize;
	if (!this->GetFileByName(&binData, &binDataSize, m_data, m_size, "FNT.bin"))
		goto end;

	file_stream stream = FileStreamBuild(binData, binDataSize);
	if (FileStreamReadUint64(&stream) != 0)
	{
		uint32_t version = FileStreamReadUint32(&stream);

		uint16_t largeCharHeight = FileStreamReadUint16(&stream);
		uint16_t smallCharHeight = FileStreamReadUint16(&stream);

		uint16_t largeEscapeCharIdx = FileStreamReadUint16(&stream);
		uint16_t smallEscapeCharIdx = FileStreamReadUint16(&stream);

		font->largeCharHeight = largeCharHeight;
		font->smallCharHeight = smallCharHeight;
		font->defaultLargeChar = largeEscapeCharIdx;
		font->defaultSmallChar = smallEscapeCharIdx;

		// padding
		FileStreamReadUint64(&stream);

		uint32_t charSizeOffset = FileStreamReadUint16(&stream) << 2;
		uint16_t charSizeCount = FileStreamReadUint16(&stream);

		uint32_t largeCharOffset = FileStreamReadUint16(&stream) << 2;
		uint16_t largeCharCount = FileStreamReadUint16(&stream);

		uint32_t smallCharOffset = FileStreamReadUint16(&stream) << 2;
		uint16_t smallCharCount = FileStreamReadUint16(&stream);

		uint32_t charSizeDataSize = (largeCharOffset - charSizeOffset);
		uint32_t largeCharDataSize = (smallCharOffset - largeCharOffset);
		uint32_t smallCharDataSize = (binDataSize - smallCharOffset);

		FileStreamSetCursor(&stream, charSizeOffset);

		size_t decompCharSizeSize;
		unsigned char* decompCharSizeData = DecompressBuffer(
			FileStreamGetAtCursor(&stream), charSizeDataSize, &decompCharSizeSize
		);

		if (decompCharSizeData == nullptr)
			goto end;

		file_stream charSizeStream = FileStreamBuild(decompCharSizeData, decompCharSizeSize);

		for (uint16_t i = 0; i < charSizeCount; i++)
		{
			uint8_t offsetX = FileStreamReadUint8(&charSizeStream);
			uint8_t offsetY = FileStreamReadUint8(&charSizeStream);

			uint8_t glyphWidth = FileStreamReadUint8(&charSizeStream);
			uint8_t glyphHeight = FileStreamReadUint8(&charSizeStream);

			res_font::CharSizeInfo charSize = { 0 };
			charSize.offsetX = offsetX;
			charSize.offsetY = offsetY;
			charSize.glyphWidth = glyphWidth;
			charSize.glyphHeight = glyphHeight;

			font->m_charSizeList.push_back(charSize);
		}

		if (largeCharCount > 0)
		{
			FileStreamSetCursor(&stream, largeCharOffset);

			size_t decompLargeCharInfoSize;
			unsigned char* decompLargeCharInfoData = DecompressBuffer(
				FileStreamGetAtCursor(&stream), largeCharDataSize, &decompLargeCharInfoSize
			);

			if (decompLargeCharInfoData == nullptr)
				goto end;

			file_stream largeCharStream = FileStreamBuild(decompLargeCharInfoData, decompLargeCharInfoSize);

			for (uint16_t i = 0; i < largeCharCount; i++)
			{
				uint16_t code = FileStreamReadUint16(&largeCharStream);
				uint16_t sizeInfo = FileStreamReadUint16(&largeCharStream);
				uint32_t imageInfo = FileStreamReadUint32(&largeCharStream);

				res_font::CharInfo charInfo = { 0 };
				charInfo.code = code;
				charInfo.width = (sizeInfo >> 10);

				charInfo.imgOffsetX = (imageInfo >> 4) & 0x3FFF;
				charInfo.imgOffsetY = (imageInfo >> 18);
				charInfo.imgIndex = (imageInfo & 0xF);

				charInfo.is_small = false;
				charInfo.bounds = &font->m_charSizeList[sizeInfo & 0x3FF];

				font->m_charInfoList.push_back(charInfo);
			}

			delete decompLargeCharInfoData;
		}

		//if (smallCharCount > 0)
		//{
		//	FileStreamSetCursor(&stream, smallCharOffset);

		//	size_t decompSmallCharInfoSize;
		//	unsigned char* decompSmallCharInfoData = DecompressBuffer(
		//		FileStreamGetAtCursor(&stream), smallCharDataSize, &decompSmallCharInfoSize
		//	);

		//	if (decompSmallCharInfoData == nullptr)
		//		goto end;

		//	file_stream smallCharStream = FileStreamBuild(decompSmallCharInfoData, decompSmallCharInfoSize);

		//	for (uint16_t i = 0; i < smallCharCount; i++)
		//	{
		//		uint16_t code = FileStreamReadUint16(&smallCharStream);
		//		uint16_t sizeInfo = FileStreamReadUint16(&smallCharStream);
		//		uint32_t imageInfo = FileStreamReadUint32(&smallCharStream);

		//		res_font::CharInfo charInfo = { 0 };
		//		charInfo.code = code;
		//		charInfo.width = (sizeInfo >> 10);

		//		charInfo.imgOffsetX = (imageInfo >> 4) & 0x3FFF;
		//		charInfo.imgOffsetY = (imageInfo >> 18);
		//		charInfo.imgIndex = (imageInfo & 0xF);

		//		charInfo.is_small = true;
		//		charInfo.bounds = &font->m_charSizeList[sizeInfo & 0x3FF];

		//		font->m_charInfoList.push_back(charInfo);
		//	}

		//	delete decompSmallCharInfoData;
		//}

		file_xi xi;
		xi.LoadFromMemory(imgData, imgDataSize);
		
		if (!xi.BuildImage(&font->m_image))
			goto end;

		delete decompCharSizeData;

		succ = true;
	}

end:
	return succ;
}
