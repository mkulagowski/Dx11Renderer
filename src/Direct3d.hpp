#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <string>
//#include <DirectXMath.h>
#include "Matrix.hpp"
#include <DirectXPackedVector.h>
#include "DxObject.hpp"

class Direct3d
{
public:
	Direct3d();
	~Direct3d();

	bool Init(uint16_t screenWidth, uint16_t screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(Vector clearColor);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	Matrix GetProjectionMatrix() const;
	void SetFoV(float fov);
	void ChangeFoV(float fov);
	void RecalculateProjectionMatrix();
	Matrix GetWorldMatrix() const;
	Matrix GetOrthoMatrix() const;

	const std::string& GetVideoCardDesc() const;
	int GetVideoCardMemory() const;

private:
	bool InitVideoCardInfo();
	bool InitSwapChain(HWND hwnd, bool fullscreen);
	bool InitRenderTargetView();
	bool InitDepthBuffer();
	bool InitDepthStencilState();
	bool InitDepthStencilView();
	bool InitRasterState();


	bool mVsyncEnabled;
	int mVideoCardMemory;
	std::string mVideoCardDescription;
	unsigned char mRefreshRate[2];

	DxObject<IDXGISwapChain> mSwapChain;
	DxObject<ID3D11Device> mDevice;
	DxObject<ID3D11DeviceContext> mDeviceContext;
	DxObject<ID3D11RenderTargetView> mRenderTargetView;
	DxObject<ID3D11Texture2D> mDepthStencilBuffer;
	DxObject<ID3D11DepthStencilState> mDepthStencilState;
	DxObject<ID3D11DepthStencilView> mDepthStencilView;
	DxObject<ID3D11RasterizerState> mRasterState;

	float mFoV;
	uint16_t mScreenWidth, mScreenHeight;
	float mScreenFar, mScreenNear;
	Matrix mProjectionMatrix;
	Matrix mWorldMatrix;
	Matrix mOrthoMatrix;
};