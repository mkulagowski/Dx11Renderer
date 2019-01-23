#pragma once

#include "Shader.hpp"

class TextureShader : public Shader
{
public:
	TextureShader();
	~TextureShader();

	bool Render(ID3D11DeviceContext * deviceContext, int indexCount, Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, ID3D11ShaderResourceView* texture);
protected:
	bool InitializeShader(ID3D11Device*, HWND) override;
	bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext *deviceContext, int indexCount) override;

	DxObject<ID3D11SamplerState> mSampleState;
};