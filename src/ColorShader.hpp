#pragma once

#include "Shader.hpp"
//#include <d3dcompiler.h>

class ColorShader : public Shader
{

public:
	ColorShader();
	~ColorShader();
	
private:
	bool InitializeShader(ID3D11Device*, HWND) override;
};