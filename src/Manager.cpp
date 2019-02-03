#include "Manager.hpp"
#include "Timer.hpp"

Manager & Manager::getInstance()
{
	static Manager instance;
	return instance;
}

bool Manager::init()
{
	// Initialize the width and height of the screen to zero before sending the variables into the function.
	int screenWidth = 0;
	int screenHeight = 0;

	// Initialize the windows api.
	initializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	mInput = std::make_unique<Input>();
	if (!mInput->Init(mHinstance, mHwnd, screenWidth, screenHeight))
		return false;
	mGraphics = std::make_unique<Graphics>();

	// Initialize the graphics object.
	return mGraphics->Init(screenWidth, screenHeight, mHwnd);
}

void Manager::run()
{
	MSG msg;
	bool done, result;
	Timer<double, std::micro> timer;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
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
			result = frame(delta);
			if (!result)
				done = true;
		}

		if (mInput->IsKeyDown(DIK_ESCAPE))
			done = true;

	}

	return;
}

Manager::Manager()
{
}

Manager::~Manager()
{
	shutdownWindows();
}
#include <cstdio>
bool Manager::frame(double delta)
{
	bool result;

	mInput->ProcessInput();
	
	// Check if the user pressed escape and wants to exit the application.
	if (mInput->IsKeyDown(DIK_ESCAPE))
	{
		return false;
	}

	Float2 mouse = mInput->GetMouseLocation();
	//printf("MOUSEX: %f, MOUSEY: %f\n", mouse.f[0], mouse.f[1]);
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
	// Do the frame processing for the graphics object.
	result = mGraphics->Render();
	if (!result)
	{
		return false;
	}

	return true;
}

void Manager::initializeWindows(int &screenWidth, int &screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get the instance of this application.
	mHinstance = GetModuleHandle(NULL);

	// Give the application a name.
	mApplicationName = "Engine";

	// Setup the windows class with default settings.
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
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
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
		screenWidth = 800;
		screenHeight = 600;

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
	ShowCursor(false);

	return;
}

void Manager::shutdownWindows()
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

LRESULT CALLBACK Manager::handleMessage(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	/*switch (umsg)
	{
		
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			mInput->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			mInput->KeyUp((unsigned int)wparam);
			return 0;
		}
		
		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}*/
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return Manager::getInstance().handleMessage(hwnd, umessage, wparam, lparam);
		}
	}
}