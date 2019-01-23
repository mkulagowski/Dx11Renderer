#pragma once

#include <d3d11.h>
#include "DxObject.hpp"
#include "Vector.hpp"
#include <string>
#include "Image.hpp"

class Texture
{
public:
	Texture();
	~Texture();

	bool Initialize(ID3D11Device* device, std::string pngFileName);
	bool Initialize(ID3D11Device* device, Vector color);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	bool CreateBuffers(ID3D11Device* device, const Image &img);

	DxObject<ID3D11ShaderResourceView> mTexture;
};