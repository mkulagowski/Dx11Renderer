#include "Shader.hpp"
#include <fstream>

Shader::Shader()
{
}

Shader::~Shader()
{
	Shutdown();
}

bool Shader::Initialize(ID3D11Device *device, HWND hwnd)
{
	// Initialize the vertex and pixel shaders.
	return InitializeShader(device, hwnd);
}

void Shader::Shutdown()
{
}

bool Shader::Render(ID3D11DeviceContext * deviceContext, int indexCount, Matrix worldMatrix,
	Matrix viewMatrix, Matrix projectionMatrix)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
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

bool Shader::SetShaderParameters(ID3D11DeviceContext *deviceContext, Matrix worldMatrix,
	Matrix viewMatrix, Matrix projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;


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
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, mMatrixBuffer.getAt());

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext *deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(mInputLayout.get());

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(mVertexShader.get(), NULL, 0);
	deviceContext->PSSetShader(mPixelShader.get(), NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
