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
	: mScreenHeight(0)
	, mScreenWidth(0)
{
}

Input::~Input()
{
}

bool Input::Init(HINSTANCE hinst, HWND hwnd, unsigned int screenWidth, unsigned int screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	// Initialize the main direct input interface.
	HRESULT result = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(mDirectInput.getAt()), NULL);
	if (FAILED(result))
		return false;

	// Initialize the direct input interface for the keyboard.
	result = mDirectInput->CreateDevice(GUID_SysKeyboard, mKeyboard.getAt(), NULL);
	if (FAILED(result))
		return false;

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return false;

	// Set the cooperative level of the keyboard to not share with other programs.
	result = mKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	// Now acquire the keyboard.
	result = mKeyboard->Acquire();
	if (FAILED(result))
		return false;

	// Initialize the direct input interface for the mouse.
	result = mDirectInput->CreateDevice(GUID_SysMouse, mMouse.getAt(), NULL);
	if (FAILED(result))
		return false;

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = mMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
		return false;

	// Set the cooperative level of the mouse to share with other programs.
	result = mMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
		return false;

	// Acquire the mouse.
	result = mMouse->Acquire();
	return !FAILED(result);
}

bool Input::IsKeyDown(unsigned int key) const
{
	return mKeys[key] & 0x80;
}

bool Input::IsMouseDown(unsigned int btn) const
{
	return mMouseState.rgbButtons[btn] & 0x80;
}

Float2 Input::GetMouseLocation() const
{
	return { static_cast<float>(mMouseState.lX), static_cast<float>(mMouseState.lY) };
}

float Input::GetMouseWheel() const
{
	return -static_cast<float>(mMouseState.lZ);
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

bool Input::ProcessInput()
{
	// Read the current state of the keyboard.
	bool result = ReadKeyboard();
	if (!result)
		return false;

	// Read the current state of the mouse.
	return ReadMouse();
}
