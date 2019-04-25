#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <memory>

#include "Utils/Input.hpp"
#include "Graphics.hpp"
#include "Light.hpp"

class Manager
{
public:
	static Manager& GetInstance();

	bool Init();
	void Run();

private:
	Manager();
	~Manager();
	bool Frame(double delta);
	void SceneSetup();
	bool FrameUI();
	void InitializeWindows(int &screenWidth, int &screenHeight);
	void ShutdownWindows();

	LPCSTR mApplicationName;
	HINSTANCE mHinstance;
	HWND mHwnd;

	std::unique_ptr<Input> mInput;
	std::unique_ptr<Graphics> mGraphics;
	std::unique_ptr<LightManager> mLights;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);