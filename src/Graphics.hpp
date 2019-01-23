#pragma once

#include <memory>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Direct3d.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "LightShader.hpp"
#include "Light.hpp"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool Init(uint16_t screenWidth, uint16_t screenHeight, HWND hwnd);
	bool Frame();
	void MoveCamera(float x, float y, double delta, Camera::Move dir);
	void ZoomCamera(float z);

private:
	//bool Render();
	bool RenderWithRotation(float rot);

	std::unique_ptr<Direct3d> mD3d;
	std::unique_ptr<Camera> mCamera;
	std::vector<std::unique_ptr<Model>> mModels;
	std::unique_ptr<LightShader> mShader;
	std::unique_ptr<Light> mLight;
};