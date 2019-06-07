#include "Shader.hpp"
#include "Logger.hpp"
#include <fstream>
#include "PixelShaderCompiled.hpp"
#include "VertexShaderCompiled.hpp"

Shader::Shader()
{
}

Shader::~Shader()
{
}

bool Shader::Initialize(ID3D11Device *device, HWND hwnd)
{
	return InitializeShader(device, hwnd);
}

void Shader::Shutdown()
{
}

bool Shader::Render(ID3D11DeviceContext * deviceContext, int indexCount, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix, Vector cameraPos, LightBufferType& light, MaterialProperties* material)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, cameraPos, light, material);
	if (!result)
	{
		LOGE("Setting shader parameters FAILED!");
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext *deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(mInputLayout.get());

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(mVertexShader.get(), NULL, 0);
	deviceContext->PSSetShader(mPixelShader.get(), NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, mSampleState.getAt());

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

bool Shader::CreateVertexShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize)
{
	// Compile the vertex shader code.
	HRESULT result = device->CreateVertexShader(shaderByteArray, shaderArraySize, nullptr, mVertexShader.getAt());

	return !FAILED(result);
}

bool Shader::CreatePixelShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize)
{
	// Compile the vertex shader code.
	HRESULT result = device->CreatePixelShader(shaderByteArray, shaderArraySize, nullptr, mPixelShader.getAt());

	return !FAILED(result);
}

bool Shader::CreateInputLayout(ID3D11Device* device, const unsigned char* vertexByteArray, const size_t vertexArraySize)
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	HRESULT result = device->CreateInputLayout(polygonLayout, numElements, vertexByteArray, vertexArraySize, mInputLayout.getAt());
	return !FAILED(result);
}

bool Shader::CreateCBuffers(ID3D11Device* device)
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

	D3D11_BUFFER_DESC materialBufferDesc;
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	materialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	materialBufferDesc.ByteWidth = sizeof(Material);
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	materialBufferDesc.MiscFlags = 0;
	materialBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&materialBufferDesc, NULL, mMaterialBuffer.getAt());
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

bool Shader::CreateTextureSampler(ID3D11Device* device)
{
	// Create a texture sampler state description.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
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

void Shader::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, LPCTSTR shaderFilename)
{
	// Get a pointer to the error message text buffer.
	char* compileErrors = reinterpret_cast<char*>(errorMessage->GetBufferPointer());

	// Get the length of the message.
	unsigned long bufferSize = static_cast<unsigned long>(errorMessage->GetBufferSize());

	// Open a file to write the error message to.
	std::ofstream fout;
	fout.open("shader-error.txt");

	// Write out the error message.
	for (unsigned long i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool Shader::InitializeShader(ID3D11Device * device, HWND hwnd)
{
	LOGI("Initializing shader!");

	if (!CreateVertexShader(device, gVertexShaderCompiledArray, sizeof(gVertexShaderCompiledArray)))
	{
		LOGE("CreateVertexShader failed!");
		return false;
	}

	if (!CreatePixelShader(device, gPixelShaderCompiledArray, sizeof(gPixelShaderCompiledArray)))
	{
		LOGE("CreatePixelShader failed!");
		return false;
	}

	if (!CreateInputLayout(device, gVertexShaderCompiledArray, sizeof(gVertexShaderCompiledArray)))
	{
		LOGE("CreateInputLayout failed!");
		return false;
	}

	if (!CreateTextureSampler(device))
	{
		LOGE("CreateTextureSampler failed!");
		return false;
	}

	if (!CreateCBuffers(device))
	{
		LOGE("CreateCBuffers failed!");
		return false;
	}
	
	LOGI("Shader initialized!");

	return true;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext * deviceContext, Matrix worldMatrix, Matrix viewMatrix,
	Matrix projectionMatrix, Vector cameraPosition, LightBufferType& light, MaterialProperties* material)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11ShaderResourceView* texPtr = nullptr;
	if (material->mMaterial.mUseTexture)
		texPtr = material->mTexture->GetShaderResourceView();

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(mMatrixBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	// Transpose the matrices to prepare them for the shader.
	Matrix view = viewMatrix;// .Transposed();
	Matrix proj = projectionMatrix;// .Transposed();
	Matrix wrl = worldMatrix;// .Transposed();
	Matrix viewProjectionMatrix1 = (wrl * view) * proj;
	Matrix viewProjectionMatrix2 = wrl * view * proj;
	Matrix viewProjectionMatrix3 = proj * view * wrl;
	Matrix viewProjectionMatrix4 = (proj * view) * wrl;
	Matrix viewProjectionMatrix5 = proj * (view * wrl);
	dataPtr->world = wrl;//.Transposed();
	dataPtr->inverseTransposeWorld = wrl.Inverted().Transposed();
	dataPtr->proj = proj;
	dataPtr->view = view;
	dataPtr->worldViewProjection = (wrl * view) * proj;// .Transposed();

	// Unlock the constant buffer.
	deviceContext->Unmap(mMatrixBuffer.get(), 0);

	// Set the position of the constant buffer in the vertex shader.
	unsigned int bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, mMatrixBuffer.getAt());

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texPtr);


	// Lock the light constant buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	result = deviceContext->Map(mLightBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
	if (FAILED(result))
	{
		LOGE("Mapping lightbuffer failed");
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	LightBufferType* lightDataPtr = (LightBufferType*)mappedResource2.pData;
	lightDataPtr->EyePosition = cameraPosition.ToFloat4();
	lightDataPtr->GlobalAmbient = light.GlobalAmbient;
	for (int i = 0; i < 8; i++)
		lightDataPtr->Lights[i] = light.Lights[i];

	// Unlock the constant buffer.
	deviceContext->Unmap(mLightBuffer.get(), 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, mLightBuffer.getAt());

	// Lock the light constant buffer so it can be written to.
	if (material)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource3;
		result = deviceContext->Map(mMaterialBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource3);
		if (FAILED(result))
		{
			LOGE("Mapping materialbuffer failed");
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		Material* dataPtrMat = (Material*)mappedResource3.pData;

		// Copy the lighting variables into the constant buffer.
		dataPtrMat->mAmbient = material->mMaterial.mAmbient;
		dataPtrMat->mDiffuse = material->mMaterial.mDiffuse;
		dataPtrMat->mEmissive = material->mMaterial.mEmissive;
		dataPtrMat->mSpecular = material->mMaterial.mSpecular;
		dataPtrMat->mSpecularPower = material->mMaterial.mSpecularPower;
		dataPtrMat->mUseTexture = material->mMaterial.mUseTexture;
		//dataPtrCam->padding = 0.f;

		// Unlock the constant buffer.
		deviceContext->Unmap(mMaterialBuffer.get(), 0);

		// Set the position of the light constant buffer in the pixel shader.
		bufferNumber = 0;

		// Finally set the light constant buffer in the pixel shader with the updated values.
		deviceContext->PSSetConstantBuffers(bufferNumber, 1, mMaterialBuffer.getAt());
	}

	return true;
}