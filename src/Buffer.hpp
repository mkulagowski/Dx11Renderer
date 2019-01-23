#pragma once
#include <cstdint>
#include<vector>

class Buffer {
	std::vector<uint8_t> mData;
	size_t mSize, mPos;

public:
	Buffer(void* data, size_t size);
	Buffer();
	~Buffer();

	void* GetData() const;
	size_t GetSize() const;
	void Release();
	bool Seek(uint64_t position);
	size_t Read(void* buffer, size_t num);
	void SetData(void* data, size_t size);

};