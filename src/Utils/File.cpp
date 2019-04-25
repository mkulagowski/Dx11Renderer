#include "File.hpp"

#include <fstream>

File::File()
	: mFileHandle(INVALID_HANDLE_VALUE)
	, mMode(AccessMode::No)
{
}

File::File(const std::string& path, AccessMode mode, bool overwrite)
	: mFileHandle(INVALID_HANDLE_VALUE)
	, mMode(AccessMode::No)
{
	Open(path, mode, overwrite);
}

File::File(File&& other)
{
	Close();

	mFileHandle = other.mFileHandle;
	other.mFileHandle = INVALID_HANDLE_VALUE;
}

File::~File()
{
	Close();
}

bool File::IsOpened() const
{
	return mFileHandle != INVALID_HANDLE_VALUE;
}

bool File::Open(const std::string& path, AccessMode access, bool overwrite)
{
	Close();


	mMode = access;
	DWORD desiredAccess;
	switch (access)
	{
	case AccessMode::Read:
		desiredAccess = GENERIC_READ;
		break;
	case AccessMode::Write:
		desiredAccess = GENERIC_WRITE;
		break;
	case AccessMode::ReadWrite:
		desiredAccess = GENERIC_READ | GENERIC_WRITE;
		break;
	default:
		mMode = AccessMode::No;
		return false;
	}

	DWORD creationDisposition;
	if (access == AccessMode::Read) // when opening for read-only, open only exising files
		creationDisposition = OPEN_EXISTING;
	else
		creationDisposition = overwrite ? CREATE_ALWAYS : OPEN_ALWAYS;

	mFileHandle = ::CreateFile(path.c_str(), desiredAccess, FILE_SHARE_READ, NULL,
		creationDisposition, FILE_ATTRIBUTE_NORMAL, 0);

	if (mFileHandle == INVALID_HANDLE_VALUE)
	{
		mMode = AccessMode::No;
		return false;
	}

	return true;
}

void File::Close()
{
	if (IsOpened())
	{
		::CloseHandle(mFileHandle);
		mFileHandle = INVALID_HANDLE_VALUE;
		mMode = AccessMode::No;
	}
}

size_t File::Read(void* data, size_t size)
{
	if (!IsOpened() || (mMode != AccessMode::Read && mMode != AccessMode::ReadWrite))
		return 0;

	DWORD toRead;
	if (size > static_cast<size_t>(MAXDWORD))
		toRead = MAXDWORD;
	else
		toRead = static_cast<DWORD>(size);

	DWORD read = 0;
	if (::ReadFile(mFileHandle, data, toRead, &read, 0) == 0)
		return 0;

	return static_cast<size_t>(read);
}

size_t File::Write(const void* data, size_t size)
{
	if (!IsOpened() || (mMode != AccessMode::Write && mMode != AccessMode::ReadWrite))
		return 0;

	DWORD toWrite;
	if (size > static_cast<size_t>(MAXDWORD))
		toWrite = MAXDWORD;
	else
		toWrite = static_cast<DWORD>(size);

	DWORD written = 0;
	if (::WriteFile(mFileHandle, data, toWrite, &written, 0) == 0)
		return 0;

	return static_cast<size_t>(written);
}

int64_t File::GetSize() const
{
	if (!IsOpened())
		return -1;

	LARGE_INTEGER size;
	if (::GetFileSizeEx(mFileHandle, &size) == 0)
		return -1;

	return static_cast<int64_t>(size.QuadPart);
}

bool File::Seek(int64_t pos, SeekMode mode)
{
	if (!IsOpened())
		return false;

	DWORD moveMethod;
	switch (mode)
	{
	case SeekMode::Begin:
		moveMethod = FILE_BEGIN;
		break;
	case SeekMode::Current:
		moveMethod = FILE_CURRENT;
		break;
	case SeekMode::End:
		moveMethod = FILE_END;
		break;
	default:
		return false;
	}

	LARGE_INTEGER posLarge;
	posLarge.QuadPart = static_cast<LONGLONG>(pos);
	if (::SetFilePointerEx(mFileHandle, posLarge, NULL, moveMethod) == 0)
		return false;

	return true;
}

int64_t File::GetPos() const
{
	if (!IsOpened())
		return -1;

	LARGE_INTEGER posLarge, pos;
	posLarge.QuadPart = 0;
	if (::SetFilePointerEx(mFileHandle, posLarge, &pos, FILE_CURRENT) == 0)
		return -1;

	return static_cast<int64_t>(pos.QuadPart);
}

AccessMode File::GetFileMode() const
{
	return mMode;
}
