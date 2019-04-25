#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>
#include "../DxObject.hpp"
#include "../Float2.hpp"

class Input
{
public:
	Input();
	~Input();

	bool Init(HINSTANCE hinst, HWND hwnd, unsigned int screenWidth, unsigned int screenHeight);

	bool ProcessInput();

	bool IsKeyDown(unsigned int key) const;
	bool IsMouseDown(unsigned int btn) const;
	Float2 GetMouseLocation() const;
	float GetMouseWheel() const;

private:
	bool ReadKeyboard();
	bool ReadMouse();

	unsigned char mKeys[256];
	unsigned int mScreenWidth, mScreenHeight;
	DIMOUSESTATE mMouseState;

	DxObject<IDirectInput8> mDirectInput;
	DxObject<IDirectInputDevice8> mKeyboard;
	DxObject<IDirectInputDevice8> mMouse;
};