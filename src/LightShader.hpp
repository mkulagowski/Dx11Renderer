
#include "TextureShader.hpp"

class LightShader : public TextureShader
{
public:
	struct CameraBufferType
	{
		Float3 cameraPosition;
		float padding;
	};
	struct LightBufferType
	{
		Float4 ambientColor;
		Float4 diffuseColor;
		Float3 lightDirection;
		float specularPower;
		Float4 specularColor;
	};
	LightShader();
	~LightShader();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, ID3D11ShaderResourceView* texture, Vector lightDirection, Vector ambientColor, Vector diffuseColor, Vector cameraPosition, Vector specularColor, float specularPower);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd) override;
	bool CreateVertexShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize);
	bool CreatePixelShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize);
	bool CreateInputLayout(ID3D11Device* device, const unsigned char* vertexByteArray, const size_t vertexArraySize);
	bool CreateTextureSampler(ID3D11Device* device);
	bool CreateCBuffers(ID3D11Device* device);

	bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix, ID3D11ShaderResourceView*, Vector, Vector, Vector, Vector cameraPosition, Vector specularColor, float specularPower);
	
	DxObject<ID3D11Buffer> mLightBuffer;
	DxObject<ID3D11Buffer> mCameraBuffer;
};