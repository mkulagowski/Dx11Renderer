#pragma once

template<typename T>
class DxObject
{
public:
	DxObject()
		: mPtr(nullptr)
	{}

	~DxObject()
	{
		if (mPtr)
			mPtr->Release();
		mPtr = nullptr;
	}

	T* get()
	{
		return mPtr;
	}

	T** getAt()
	{
		return &mPtr;
	}

	operator bool() const
	{
		return static_cast<bool>(mPtr);
	}
	/*
	operator T*() const
	{
		return mPtr;
	}

	operator T**() const
	{
		return &mPtr;
	}
	*/
	T* operator->() const
	{
		return mPtr;
	}

private:
	T* mPtr;
};
