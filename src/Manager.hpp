#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>

#include "Input.hpp"
#include "Graphics.hpp"

class Manager
{
public:
	static Manager& getInstance();

	bool init();
	void run();
	LRESULT CALLBACK handleMessage(HWND, UINT, WPARAM, LPARAM);

private:
	Manager();
	~Manager();
	bool frame(double delta);
	void initializeWindows(int&, int&);
	void shutdownWindows();

	LPCSTR mApplicationName;
	HINSTANCE mHinstance;
	HWND mHwnd;

	std::unique_ptr<Input> mInput;
	std::unique_ptr<Graphics> mGraphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);