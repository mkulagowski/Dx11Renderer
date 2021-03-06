#include "Texture.hpp"
#include "Logger.hpp"

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device *device, std::string filename)
{
	Image img;
	if (!img.OpenPng(filename))
		return false;
	
	return CreateBuffers(device, img);
}

bool Texture::Initialize(ID3D11Device *device, Vector v)
{
	Image img;

	unsigned char col[4]{
		static_cast<unsigned char>(v.f[0] * 255) ,
		static_cast<unsigned char>(v.f[1] * 255) ,
		static_cast<unsigned char>(v.f[2] * 255) ,
		static_cast<unsigned char>(v.f[3] * 255)
	};
	img.SetData(&col, sizeof(col), 1, 1);

	return CreateBuffers(device, img);
}

ID3D11ShaderResourceView * Texture::GetShaderResourceView()
{
	return mTexture.get();
}

bool Texture::CreateBuffers(ID3D11Device * device, const Image &img)
{
	HRESULT result = 0;
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0; // No CPU access required.
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Width = static_cast<UINT>(img.GetWidth());
	textureDesc.Height = static_cast<UINT>(img.GetHeight());
	//textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA texData;
	ZeroMemory(&texData, sizeof(D3D11_SUBRESOURCE_DATA));
	texData.pSysMem = img.GetData();
	texData.SysMemPitch = (UINT)(img.GetWidth() * img.GetChannels());
	texData.SysMemSlicePitch = (UINT)(img.GetWidth() * img.GetHeight() * img.GetChannels());
	
	DxObject<ID3D11Texture2D> texPtr;
	result = device->CreateTexture2D(&textureDesc, nullptr, texPtr.getAt()); //< give nullptr to generate mips

	if (FAILED(result))
	{
		LOGE("CreateTexture2D failed");
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resViewDesc;
	ZeroMemory(&resViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	resViewDesc.Format = textureDesc.Format;
	resViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resViewDesc.Texture2D.MostDetailedMip = 0;
	resViewDesc.Texture2D.MipLevels = -1;


	result = device->CreateShaderResourceView(texPtr.get(), &resViewDesc, mTexture.getAt());

	if (FAILED(result))
	{
		LOGE("CreateShaderResourceView failed");
		return false;
	}

	DxObject<ID3D11DeviceContext> ctxPtr;
	device->GetImmediateContext(ctxPtr.getAt());
	ctxPtr->UpdateSubresource(texPtr.get(), 0, 0, img.GetData(), texData.SysMemPitch, 0);
	ctxPtr->GenerateMips(mTexture.get());

	return true;
}
