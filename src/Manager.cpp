#include "Manager.hpp"
#include "Timer.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

Manager & Manager::GetInstance()
{
	static Manager instance;
	return instance;
}

bool Manager::Init()
{
	// Initialize the width and height of the screen to zero before sending the variables into the function.
	int screenWidth = 0;
	int screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	mInput = std::make_unique<Input>();
	if (!mInput->Init(mHinstance, mHwnd, screenWidth, screenHeight))
		return false;

	// Initialize the graphics object.
	mGraphics = std::make_unique<Graphics>();
	if (!mGraphics->Init(screenWidth, screenHeight, mHwnd))
		return false;


	SceneSetup();
	return true;
}

void Manager::Run()
{
	MSG msg;
	bool result;
	Timer<double, std::micro> timer;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	bool done = false;
	timer.Start();
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		} else
		{
			double delta = timer.Stop().count() / 1000000;
			// Otherwise do the frame processing.
			result = Frame(delta);
			if (!result)
				done = true;
		}

		if (mInput->IsKeyDown(DIK_ESCAPE))
			done = true;
	}
}

Manager::Manager()
{
}

Manager::~Manager()
{
	ShutdownWindows();
}

bool Manager::Frame(double delta)
{
	mInput->ProcessInput();
	
	// Check if the user pressed escape and wants to exit the application.
	if (mInput->IsKeyDown(DIK_ESCAPE))
	{
		return false;
	}

	if (mInput->IsMouseDown(1))
	{
		Float2 mouse = mInput->GetMouseLocation();
		bool anyKey = false;
		if (mInput->IsKeyDown(DIK_UP) || mInput->IsKeyDown(DIK_W))
		{
			mGraphics->MoveCamera(mouse.f[0], mouse.f[1], delta, Camera::Move::Fwd);
			anyKey = true;
		}

		if (mInput->IsKeyDown(DIK_DOWN) || mInput->IsKeyDown(DIK_S))
		{
			mGraphics->MoveCamera(mouse.f[0], mouse.f[1], delta, Camera::Move::Bwd);
			anyKey = true;
		}

		if (mInput->IsKeyDown(DIK_LEFT) || mInput->IsKeyDown(DIK_A))
		{
			mGraphics->MoveCamera(mouse.f[0], mouse.f[1], delta, Camera::Move::Lt);
			anyKey = true;
		}

		if (mInput->IsKeyDown(DIK_RIGHT) || mInput->IsKeyDown(DIK_D))
		{
			mGraphics->MoveCamera(mouse.f[0], mouse.f[1], delta, Camera::Move::Rt);
			anyKey = true;
		}

		mGraphics->ZoomCamera(static_cast<float>(mInput->GetMouseWheel() * delta * 0.1f));

		if (!anyKey)
			mGraphics->MoveCamera(mouse.f[0], mouse.f[1], delta, Camera::Move::None);
	}

	FrameUI();

	// Do the frame processing for the graphics object.
	return mGraphics->Render();
}

void Manager::SceneSetup()
{
	// Set up lights for the scene
	LightManager* lm = mGraphics->GetLight();
	lm->SetAmbient({ 0.1f, 0.1f, 0.1f, 1.0f });

	static const LightType LightTypes[LIGHT_NO] = {
		PointLight, SpotLight, SpotLight, PointLight, SpotLight, SpotLight, SpotLight, PointLight
	};

	static const bool LightEnabled[LIGHT_NO] = {
		false, false, false, false, true, false, false, false
	};


	float radius = 8.0f;
	float offset = 2.0f * PI / LIGHT_NO;
	for (int i = 0; i < LIGHT_NO; ++i)
	{
		Light* light = lm->GetLight(i);
		light->Enabled = static_cast<int>(LightEnabled[i]);
		light->LightType = LightTypes[i];
		Vector col = Vector(1, 0, 1, 1);//((i + 1) % 3, (i + 1) % 2, (i + 1), 1);
		light->Color = col.ToFloat4();
		light->SpotAngle = RADS(45.f);
		light->ConstantAttenuation = 0.5f;
		light->LinearAttenuation = 0.08f;
		light->QuadraticAttenuation = 0.0f;
		Vector LightPosition = Vector(std::sin(offset * i) * radius, 9.0f, std::cos(offset * i) * radius, 1.0f);
		light->Position = LightPosition.ToFloat4();
		Vector LightDirection(-LightPosition.f[0], -LightPosition.f[1], -LightPosition.f[2], 0.0f);
		LightDirection = LightDirection.Normalized3();
		light->Direction = LightDirection.ToFloat4();
	}

	lm->InitModels(mGraphics->GetDevice());
}

bool Manager::FrameUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Lights panel");
	mGraphics->GetLight()->RenderUI(mGraphics->GetDevice());
	ImGui::End();

	return true;
}

void Manager::InitializeWindows(int &screenWidth, int &screenHeight)
{
	// Get the instance of this application.
	mHinstance = GetModuleHandle(NULL);

	// Give the application a name.
	mApplicationName = "Engine";

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	int posX, posY;
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 1024;
		screenHeight = 768;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	mHwnd = CreateWindowEx(WS_EX_APPWINDOW, mApplicationName, mApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, mHinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(mHwnd, SW_SHOW);
	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);

	// Hide the mouse cursor.
	//ShowCursor(false);
}

void Manager::ShutdownWindows()
{
	if (mHwnd != NULL)
	{
		// Show the mouse cursor.
		ShowCursor(true);

		// Fix the display settings if leaving full screen mode.
		if (FULL_SCREEN)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		// Remove the window.
		DestroyWindow(mHwnd);
		mHwnd = NULL;

		// Remove the application instance.
		UnregisterClass(mApplicationName, mHinstance);
		mHinstance = NULL;
	}
	return;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, umessage, wparam, lparam))
		return true;

	switch (umessage)
	{
		case WM_SYSCOMMAND:
			if ((wparam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;

		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd, umessage, wparam, lparam);
}