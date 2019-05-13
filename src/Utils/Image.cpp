#include "Image.hpp"
#include "png.h"
#include "File.hpp"
#include "Logger.hpp"
#include <memory>

#define PNGSIGSIZE 8

namespace {

	struct pngReadInfo
	{
		char* data;
		size_t offset;
	};

	bool usingTrnsChunk(png_structp pngPtr, png_infop infoPtr)
	{
		
		if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
		{
			png_set_tRNS_to_alpha(pngPtr);
			return true;
		}

		return false;
	}

	void userReadData(png_structp pngPtr, png_bytep data, png_size_t length)
	{
		png_voidp a = png_get_io_ptr(pngPtr);
		pngReadInfo* info = static_cast<pngReadInfo*>(a);
		void* infoData = info->data;
		File* buff = static_cast<File*>(infoData);
		if (buff)
			info->offset += buff->Read(data, length);
	}

	bool Check(Buffer* buff)
	{
		uint32_t signature = 0;
		buff->Seek(0);
		if (sizeof(signature) < buff->Read(&signature, sizeof(signature)))
			return false;

		buff->Seek(0);

		return signature == 0x474E5089;
	}
}

Image::Image()
{
}

Image::~Image()
{
}

bool Image::OpenPng(std::string path)
{
	File inFile(path.c_str(), AccessMode::Read, false);
	uint8_t signature[PNGSIGSIZE];
	if (inFile.Read(signature, PNGSIGSIZE) != PNGSIGSIZE)
		return false;

	// verify png signature
	if (!png_check_sig(static_cast<png_const_bytep>(signature), PNGSIGSIZE))
		return false;

	// reading struct
	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!pngPtr)
		return false;

	// info struct
	png_infop infoPtr = png_create_info_struct(pngPtr);
	if (!infoPtr)
	{
		png_destroy_read_struct(&pngPtr, nullptr, nullptr);
		return false;
	}

	if (setjmp(png_jmpbuf(pngPtr)))
	{
		png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
		return false;
	}

	// set up reading callback
	pngReadInfo readInfo;
	readInfo.offset = PNGSIGSIZE;
	readInfo.data = (char*)&inFile;
	png_set_read_fn(pngPtr, static_cast<png_voidp>(&readInfo), userReadData);

	// Set the amount signature bytes
	png_set_sig_bytes(pngPtr, PNGSIGSIZE);

	// Now call png_read_info to receive the file info.
	png_read_info(pngPtr, infoPtr);

	png_uint_32 imgWidth = png_get_image_width(pngPtr, infoPtr);
	png_uint_32 imgHeight = png_get_image_height(pngPtr, infoPtr);

	png_uint_32 bitdepth = png_get_bit_depth(pngPtr, infoPtr);
	png_uint_32 channels = png_get_channels(pngPtr, infoPtr);
	png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);

	// when 16bpp then scale it down to 8bp
	if (bitdepth > 8)
		png_set_scale_16(pngPtr);

	switch (color_type)
	{
	case PNG_COLOR_TYPE_PALETTE:
		png_set_palette_to_rgb(pngPtr);
		channels = 3;

		// if the image has a transperancy set, convert it to a full Alpha channel
		if (usingTrnsChunk(pngPtr, infoPtr))
		{
			channels = 4;
		}
		break;

	case PNG_COLOR_TYPE_GRAY:

		// if the image has a transperancy set, convert it to a full Alpha channel
		if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
		{
			png_set_gray_to_rgb(pngPtr);
			channels = 4;
		}
		else
		{
			png_set_expand_gray_1_2_4_to_8(pngPtr);
		}
		break;

	case PNG_COLOR_TYPE_RGB:
		// if the image has a transperancy set, convert it to a full Alpha channel
		if (usingTrnsChunk(pngPtr, infoPtr))
		{
			channels = 4;
		}
		break;

	case PNG_COLOR_TYPE_RGBA:
		break;

	case PNG_COLOR_TYPE_GA:
		// Change G->RGB or GA->RGBA
		png_set_gray_to_rgb(pngPtr);
		channels = 4;
		break;

	default:
		return false;
	}

	size_t dataSize = imgWidth * imgHeight * bitdepth * channels / 8;
	std::unique_ptr<png_bytep[]> rowPtrs(new (std::nothrow) png_bytep[imgHeight]);
	std::unique_ptr<uint8_t[]> dataPtr(new (std::nothrow) uint8_t[dataSize]);

	if (!rowPtrs.get() || !dataPtr.get())
		return false;

	// row length in bytes
	const unsigned int stride = imgWidth * bitdepth * channels / 8;

	// A loop to set all row pointers to the starting
	// adresses for every row in the buffer
	for (png_uint_32 i = 0; i < imgHeight; i++)
	{
		png_uint_32 q = i * stride;
		rowPtrs[i] = static_cast<png_bytep>(static_cast<uint8_t*>(dataPtr.get()) + q);
	}

	// Read the imagedata and write it to the adresses pointed to
	// by rowptrs
	png_read_image(pngPtr, rowPtrs.get());

	// Create mipmap based on data read from PNG
	mWidth = imgWidth;
	mHeight = imgHeight;
	mChannels = channels;
	mImage.SetData(dataPtr.get(), dataSize);

	if (mChannels < 4)
		ExpandWithAlpha(0);

	png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);

	return true;
}

void Image::SetData(void* data, size_t size, uint64_t width, uint64_t height)
{
	SetData(data, size, width, height, 4);
}

void Image::SetData(void* data, size_t size, uint64_t width, uint64_t height, uint8_t channels)
{
	mImage.SetData(data, size);
	mWidth = width;
	mHeight = height;
	mChannels = channels;
}

uint64_t Image::GetHeight() const
{
	return mHeight;
}

uint64_t Image::GetWidth() const
{
	return mWidth;
}

uint8_t Image::GetChannels() const
{
	return mChannels;
}

void* Image::GetData() const
{
	return mImage.GetData();
}

void Image::ExpandWithAlpha(uint8_t alphaValue)
{
	if (mChannels == 4)
		return;
	LOGI("Expanding alpha channel!");
	std::vector<uint8_t> newData;
	const uint64_t newSize = mWidth * mHeight * 4;
	// Fill new image buffer with alpha values
	newData.resize(newSize, alphaValue);
	uint8_t* oldDataPtr = reinterpret_cast<uint8_t*>(mImage.GetData());
	uint8_t* newDataPtr = newData.data();

	const uint8_t oldPixSize = sizeof(newData[0]) * mChannels;
	const uint8_t newPixSize = sizeof(newData[0]) * 4;
	// For each pixel copy over the RGB values (alpha values already in vec won't get overwritten)
	for (uint64_t i = 0; i < mWidth * mHeight; i++)
		memcpy(newDataPtr + (i * newPixSize), oldDataPtr + (i * oldPixSize), oldPixSize);

	mImage.SetData(newData.data(), newSize * sizeof(newData[0]));
	mChannels = 4;
}
