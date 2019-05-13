#include "Direct3d.hpp"
#include <vector>
#include "MathUtils.hpp"
#include "Float4.hpp"
#include "Logger.hpp"

Direct3d::Direct3d()
{
}

Direct3d::~Direct3d()
{
	Shutdown();
}

bool Direct3d::InitVideoCardInfo()
{
	// Create a DirectX graphics interface factory.
	DxObject<IDXGIFactory> factory;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.getAt()));
	if (FAILED(result))
	{
		LOGE("CreateDXGIFactory failed!");
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	DxObject<IDXGIAdapter> adapter;
	result = factory->EnumAdapters(0, adapter.getAt());
	if (FAILED(result))
	{
		LOGE("EnumAdapters failed!");
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	DxObject<IDXGIOutput> adapterOutput;
	result = adapter->EnumOutputs(0, adapterOutput.getAt());
	if (FAILED(result))
	{
		LOGE("EnumOutputs failed!");
		return false;
	}

	unsigned int numModes = 0;
	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		LOGE("GetDisplayModeList1 failed!");
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	std::vector<DXGI_MODE_DESC> displayModeList(numModes);
	
	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.data());
	if (FAILED(result))
	{
		LOGE("GetDisplayModeList2 failed!");
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	mRefreshRate[0] = 0;
	mRefreshRate[1] = 0;
	for (const auto &i : displayModeList)
	{
		if (i.Width == mScreenWidth && i.Height == mScreenHeight)
		{
			mRefreshRate[0] = i.RefreshRate.Numerator;
			mRefreshRate[1] = i.RefreshRate.Denominator;
			break;
		}
	}

	// Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		LOGE("GetDesc failed!");
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	mVideoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / (1024 * 1024));

	// Convert the name of the video card to a character array and store it.
	size_t stringLength = 0;
	mVideoCardDescription.resize(128); //< array size of DXGI_ADAPTER_DESC.Description
	errno_t error = wcstombs_s(&stringLength, mVideoCardDescription.data(), 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		LOGE("Getting GPU description failed!");
		return false;
	}

	return true;
}

bool Direct3d::InitSwapChain(HWND hwnd, bool fullscreen)
{
	// Initialize the swap chain description.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = mScreenWidth;
	swapChainDesc.BufferDesc.Height = mScreenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (mVsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = mRefreshRate[0];
		swapChainDesc.BufferDesc.RefreshRate.Denominator = mRefreshRate[1];
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Turn fullscreen on/off
	swapChainDesc.Windowed = !fullscreen;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	
	UINT creationFlags = 0;

#ifdef _DEBUG
	// If the project is in a debug build, enable the debug layer.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create the swap chain, Direct3D device, and Direct3D device context.
	HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, mSwapChain.getAt(), mDevice.getAt(), nullptr, mDeviceContext.getAt());
	if (FAILED(result))
		return false;

	return true;
}

bool Direct3d::InitRenderTargetView()
{
	// Get the pointer to the back buffer.
	DxObject<ID3D11Texture2D> backBufferPtr;
	HRESULT result = mSwapChain.get()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferPtr.getAt()));
	if (FAILED(result))
		return false;

	// Create the render target view with the back buffer pointer.
	result = mDevice.get()->CreateRenderTargetView(backBufferPtr.get(), NULL, mRenderTargetView.getAt());
	if (FAILED(result))
		return false;

	return true;
}

bool Direct3d::InitDepthBuffer()
{
	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = mScreenWidth;
	depthBufferDesc.Height = mScreenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	HRESULT result = mDevice.get()->CreateTexture2D(&depthBufferDesc, NULL, mDepthStencilBuffer.getAt());
	if (FAILED(result))
		return false;

	return true;
}

bool Direct3d::InitDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	// Create the depth stencil state.
	HRESULT result = mDevice.get()->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.getAt());
	if (FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	mDeviceContext.get()->OMSetDepthStencilState(mDepthStencilState.get(), 1);

	return true;
}

bool Direct3d::InitDepthStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Initailze the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	HRESULT result = mDevice.get()->CreateDepthStencilView(mDepthStencilBuffer.get(), &depthStencilViewDesc, mDepthStencilView.getAt());
	if (FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	mDeviceContext.get()->OMSetRenderTargets(1, mRenderTargetView.getAt(), mDepthStencilView.get());

	return true;
}

bool Direct3d::InitRasterState()
{
	D3D11_RASTERIZER_DESC rasterDesc;
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	HRESULT result = mDevice.get()->CreateRasterizerState(&rasterDesc, mRasterState.getAt());
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	mDeviceContext.get()->RSSetState(mRasterState.get());

	return true;
}

bool Direct3d::Init(uint16_t screenWidth, uint16_t screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	LOGI("Initializing DirectX3D");
	// Store the vsync setting.
	mVsyncEnabled = vsync;
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mScreenNear = screenNear;
	mScreenFar = screenDepth;
	mFoV = PI4;

	// Get video card info (refresh rate, description, memory)
	if (!InitVideoCardInfo())
	{
		LOGE("InitVideoCardInfo failed!");
		return false;
	}

	// Initialize DirectX components
	if (!InitSwapChain(hwnd, fullscreen))
	{
		LOGE("InitSwapChain failed!");
		return false;
	}

	if (!InitRenderTargetView())
	{
		LOGE("InitRenderTargetView failed!");
		return false;
	}

	if (!InitDepthBuffer())
	{
		LOGE("InitDepthBuffer failed!");
		return false;
	}

	if (!InitDepthStencilState())
	{
		LOGE("InitDepthStencilState failed!");
		return false;
	}

	if (!InitDepthStencilView())
	{
		LOGE("InitDepthStencilView failed!");
		return false;
	}

	if (!InitRasterState())
	{
		LOGE("InitRasterState failed!");
		return false;
	}

	// Setup the viewport for rendering
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	mDeviceContext.get()->RSSetViewports(1, &viewport);

	// Create the projection matrix for 3D rendering.
	RecalculateProjectionMatrix();

	// Initialize the world matrix to the identity matrix.
	mWorldMatrix = Matrix();

	// Create an orthographic projection matrix for 2D rendering.
	mOrthoMatrix = Matrix::MakeOrtho(-static_cast<float>(screenWidth) / 2, static_cast<float>(screenWidth) / 2, -static_cast<float>(screenHeight) / 2, static_cast<float>(screenHeight) / 2, screenNear, screenDepth);

	LOGI("DirectX3D initialized!");
	return true;
}

void Direct3d::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (mSwapChain)
		mSwapChain.get()->SetFullscreenState(false, NULL);

	return;
}

void Direct3d::BeginScene(Vector clearColor)
{
	Float4 color = clearColor.ToFloat4();

	// Clear the back buffer.
	mDeviceContext.get()->ClearRenderTargetView(mRenderTargetView.get(), reinterpret_cast<const float*>(&color));

	// Clear the depth buffer.
	mDeviceContext.get()->ClearDepthStencilView(mDepthStencilView.get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void Direct3d::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (mVsyncEnabled)
	{
		// Lock to screen refresh rate.
		mSwapChain.get()->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		mSwapChain.get()->Present(0, 0);
	}

	return;
}

ID3D11Device * Direct3d::GetDevice()
{
	return mDevice.get();
}

ID3D11DeviceContext * Direct3d::GetDeviceContext()
{
	return mDeviceContext.get();
}

Matrix Direct3d::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}


void Direct3d::SetFoV(float fov)
{
	mFoV = fov;
}

void Direct3d::ChangeFoV(float fov)
{
	mFoV += fov;
}

void Direct3d::RecalculateProjectionMatrix()
{
	// Create the projection matrix for 3D rendering.
	mProjectionMatrix = Matrix::MakePerspective(static_cast<float>(mScreenWidth) / mScreenHeight, mFoV, mScreenFar, mScreenNear);
}

Matrix Direct3d::GetWorldMatrix() const
{
	return mWorldMatrix;
}

Matrix Direct3d::GetOrthoMatrix() const
{
	return mOrthoMatrix;
}

const std::string& Direct3d::GetVideoCardDesc() const
{
	return mVideoCardDescription;
}

int Direct3d::GetVideoCardMemory() const
{
	return mVideoCardMemory;
}
