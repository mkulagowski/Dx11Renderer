#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <string>

enum class SeekMode
{
	Begin,   ///< relative to file's beginning
	Current, ///< relative to current cursor position
	End,     ///< relative to file's end
};
enum class AccessMode
{
	No,
	Read,
	Write,
	ReadWrite,
};

class File
{
private:
	HANDLE mFileHandle;
	AccessMode mMode;

public:
	File();
	File(const std::string& path, AccessMode mode, bool overwrite = false);
	File(File&& other);
	~File();

	bool virtual IsOpened() const;

	bool virtual Open(const std::string& path, AccessMode access, bool overwrite = false);

	void virtual Close();

	size_t virtual Read(void* data, size_t size);

	size_t virtual Write(const void* data, size_t size);

	int64_t virtual GetSize() const;

	bool virtual Seek(int64_t pos, SeekMode mode);

	int64_t virtual GetPos() const;

	AccessMode virtual GetFileMode() const;
};