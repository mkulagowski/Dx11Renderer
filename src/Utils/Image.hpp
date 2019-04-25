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
	uint64_t GetHeight() const;
	uint64_t GetWidth() const;
	void* GetData() const;

private:
	uint64_t mHeight, mWidth;
	Buffer mImage;
};