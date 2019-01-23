#include "Buffer.hpp"

#include <memory>

Buffer::Buffer(void * data, size_t size)
	: Buffer()
{
	SetData(data, size);
}

Buffer::Buffer()
	: mSize(0)
	, mPos(0)
{
}

Buffer::~Buffer()
{
	Release();
}

void* Buffer::GetData() const
{
	return (void*)mData.data();
}

size_t Buffer::GetSize() const
{
	return mSize;
}

bool Buffer::Seek(uint64_t position)
{
	if (position < mSize)
	{
		mPos = static_cast<size_t>(position);
		return true;
	}
	return false;
}

size_t Buffer::Read(void * buffer, size_t num)
{
	if (mData.empty())
		return 0;

	if (mPos + num > mSize)
	{
		size_t maxReadable = static_cast<size_t>(mSize - mPos);
		memcpy(buffer, reinterpret_cast<const char*>(mData.data()) + mPos, maxReadable);
		mPos = mSize;
		return maxReadable;
	}
	else
	{
		memcpy(buffer, reinterpret_cast<const char*>(mData.data()) + mPos, num);
		mPos += num;
		return num;
	}
}
void Buffer::Release()
{
	mData.clear();
	mSize = 0;
	mPos = 0;
}

void Buffer::SetData(void * data, size_t size)
{
	Release();
	mData.resize(size);
	mSize = size;
	memcpy(mData.data(), data, size);
	mPos = 0;
}
