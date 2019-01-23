#include "Input.hpp"


template<>
DxObject<IDirectInputDevice8>::~DxObject()
{
	if (mPtr) {
		mPtr->Unacquire();
		mPtr->Release();
	}

	mPtr = nullptr;
}

Input::Input()
{
}

Input::~Input()
{
}

bool Input::Init(HINSTANCE hinst, HWND hwnd, unsigned int screenWidth, unsigned int screenHeight)
{
	mMouseY = 0.f;
	mMouseX = 0.f;
	mMouseZ = 0.f;
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Initialize the main direct input interface.
	HRESULT result = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(mDirectInput.getAt()), NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = mDirectInput->CreateDevice(GUID_SysKeyboard, mKeyboard.getAt(), NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}


	// Set the cooperative level of the keyboard to not share with other programs.
	result = mKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = mKeyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}



	// Initialize the direct input interface for the mouse.
	result = mDirectInput->CreateDevice(GUID_SysMouse, mMouse.getAt(), NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = mMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = mMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = mMouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Input::IsKeyDown(unsigned int key)
{
	return mKeys[key] & 0x80;
}

Float2 Input::GetMouseLocation()
{
	Float2 res(mMouseX, mMouseY);
	//mMouseX = 0.f;
	//mMouseY = 0.f;
	return res;
}

//#include <cstdio>
float Input::GetMouseWheel()
{
	//printf("MouseZ: %f\n", -mMouseZ);
	return -mMouseZ;
}

bool Input::ReadKeyboard()
{
	// Read the keyboard device.
	HRESULT result = mKeyboard->GetDeviceState(sizeof(mKeys), &mKeys);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			mKeyboard->Acquire();
		else
			return false;
	}

	return true;
}

bool Input::ReadMouse()
{
	// Read the mouse device.
	HRESULT result = mMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			mMouse->Acquire();
		else
			return false;
	}

	return true;
}
//#include <cstdio>
bool Input::ProcessInput()
{
	// Read the current state of the keyboard.
	bool result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if (!result)
	{
		return false;
	}


	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	mMouseX = static_cast<float>(mMouseState.lX);
	mMouseY = static_cast<float>(mMouseState.lY);
	mMouseZ = static_cast<float>(mMouseState.lZ);
	/*
	// Ensure the mouse location doesn't exceed the screen width or height.
	
	if (mMouseX < 0)
		mMouseX = 0;

	if (mMouseY < 0)
		mMouseY = 0;

	if (mMouseX > mScreenWidth)
		mMouseX = mScreenWidth;

	if (mMouseY > mScreenHeight)
		mMouseY = mScreenHeight;
		*/
	//printf("MouseX: %f, MouseY: %f\n", mMouseX, mMouseY);
	return true;
}
