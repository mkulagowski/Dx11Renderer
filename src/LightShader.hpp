
#include "TextureShader.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "Float3.hpp"

class LightShader : public TextureShader
{
public:
	struct CameraBufferType
	{
		Float3 cameraPosition;
		float padding;
	};
	
	LightShader();
	~LightShader();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, Vector camera, LightBufferType& light, MaterialProperties* mat);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd) override;
	bool CreateVertexShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize);
	bool CreatePixelShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize);
	bool CreateInputLayout(ID3D11Device* device, const unsigned char* vertexByteArray, const size_t vertexArraySize);
	bool CreateTextureSampler(ID3D11Device* device);
	bool CreateCBuffers(ID3D11Device* device);

	bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix, Vector cameraPosition, LightBufferType& light, MaterialProperties* mat);
	
	DxObject<ID3D11Buffer> mLightBuffer;
	DxObject<ID3D11Buffer> mCameraBuffer;
	DxObject<ID3D11Buffer> mMaterialBuffer;
};