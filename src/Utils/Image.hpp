#pragma once

#include "Buffer.hpp"
#include <string>

class Image
{
public:
	Image();
	~Image();

	bool OpenPng(std::string path);
	void SetData(void* data, size_t size, uint64_t width, uint64_t height);
	void SetData(void* data, size_t size, uint64_t width, uint64_t height, uint8_t channels);
	uint64_t GetHeight() const;
	uint64_t GetWidth() const;
	uint8_t GetChannels() const;
	void* GetData() const;

	void ExpandWithAlpha(uint8_t alphaValue);

private:
	uint64_t mHeight, mWidth;
	uint8_t mChannels;
	Buffer mImage;
};