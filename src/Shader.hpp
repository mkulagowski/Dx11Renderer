#pragma once

#include "Matrix.hpp"
#include "DxObject.hpp"
#include <d3d11.h>

class Shader
{
public:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix inverseTransposeWorld;
		Matrix worldViewProjection;
		Matrix proj;
		Matrix view;
	};

	Shader();
	~Shader();
	virtual bool Initialize(ID3D11Device*, HWND);
	virtual void Shutdown();
	virtual bool Render(ID3D11DeviceContext*, int, Matrix, Matrix, Matrix);

protected:
	virtual bool InitializeShader(ID3D11Device*, HWND) = 0;
	virtual void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCTSTR);

	virtual bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix);
	virtual void RenderShader(ID3D11DeviceContext*, int);

	DxObject<ID3D11VertexShader> mVertexShader;
	DxObject<ID3D11PixelShader> mPixelShader;
	DxObject<ID3D11InputLayout> mInputLayout;
	DxObject<ID3D11Buffer> mMatrixBuffer;
};