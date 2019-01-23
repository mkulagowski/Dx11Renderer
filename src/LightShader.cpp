#include "LightShader.hpp"
#include "shaders/light_ps.hpp"
#include "shaders/light_vs.hpp"

LightShader::LightShader()
{
}

LightShader::~LightShader()
{
}

bool LightShader::Render(ID3D11DeviceContext * deviceContext, int indexCount, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix, ID3D11ShaderResourceView * texture, Vector lightDirection, Vector ambientColor, Vector diffuseColor, Vector cameraPosition, Vector specularColor, float specularPower)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShader::CreateVertexShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize)
{
	// Compile the vertex shader code.
	HRESULT result = device->CreateVertexShader(shaderByteArray, shaderArraySize, nullptr, mVertexShader.getAt());

	return !FAILED(result);
}

bool LightShader::CreatePixelShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize)
{
	// Compile the vertex shader code.
	HRESULT result = device->CreatePixelShader(shaderByteArray, shaderArraySize, nullptr, mPixelShader.getAt());

	return !FAILED(result);
}

bool LightShader::CreateInputLayout(ID3D11Device* device, const unsigned char* vertexByteArray, const size_t vertexArraySize)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	// Create the vertex input layout description.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	HRESULT result = device->CreateInputLayout(polygonLayout, numElements, vertexByteArray, vertexArraySize, mInputLayout.getAt());
	return !FAILED(result);
}

bool LightShader::CreateCBuffers(ID3D11Device* device)
{
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = device->CreateBuffer(&matrixBufferDesc, NULL, mMatrixBuffer.getAt());
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC cameraBufferDesc;
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, mCameraBuffer.getAt());
	if (FAILED(result))
	{
		return false;
	}


	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, mLightBuffer.getAt());
	return !FAILED(result);
}

bool LightShader::CreateTextureSampler(ID3D11Device* device)
{
	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	HRESULT result = device->CreateSamplerState(&samplerDesc, mSampleState.getAt());
	return !FAILED(result);
}

bool LightShader::InitializeShader(ID3D11Device * device, HWND hwnd)
{
	if (!CreateVertexShader(device, gLightVSArray, sizeof(gLightVSArray)))
		return false;

	if (!CreatePixelShader(device, gLightPSArray, sizeof(gLightPSArray)))
		return false;

	if (!CreateInputLayout(device, gLightVSArray, sizeof(gLightVSArray)))
		return false;

	if (!CreateTextureSampler(device))
		return false;

	if (!CreateCBuffers(device))
		return false;
	
	return true;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext * deviceContext, Matrix worldMatrix, Matrix viewMatrix,
	Matrix projectionMatrix, ID3D11ShaderResourceView* texture, Vector lightDirection, Vector ambientColor,
	Vector diffuseColor, Vector cameraPosition, Vector specularColor, float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	if (!TextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture))
		return false;

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(mLightBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	LightBufferType* dataPtr = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr->ambientColor = Float4(ambientColor);
	dataPtr->diffuseColor = Float4(diffuseColor);
	dataPtr->lightDirection = Float3(lightDirection);
	dataPtr->specularColor = Float4(specularColor);
	dataPtr->specularPower = specularPower;

	// Unlock the constant buffer.
	deviceContext->Unmap(mLightBuffer.get(), 0);

	// Set the position of the light constant buffer in the pixel shader.
	unsigned int bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, mLightBuffer.getAt());

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(mCameraBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	CameraBufferType* dataPtrCam = (CameraBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtrCam->cameraPosition = Float3(cameraPosition);
	dataPtrCam->padding = 0.f;

	// Unlock the constant buffer.
	deviceContext->Unmap(mCameraBuffer.get(), 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, mCameraBuffer.getAt());


	return true;
}