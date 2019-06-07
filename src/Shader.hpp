
#include "Matrix.hpp"
#include "DxObject.hpp"
#include <d3d11.h>
#include "Material.hpp"
#include "Light.hpp"
#include "Float3.hpp"

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

	struct CameraBufferType
	{
		Float3 cameraPosition;
		float padding;
	};
	
	Shader();
	~Shader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix, Vector camera, LightBufferType& light, MaterialProperties* mat);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd);
	bool CreateVertexShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize);
	bool CreatePixelShader(ID3D11Device* device, const unsigned char* shaderByteArray, const size_t shaderArraySize);
	bool CreateInputLayout(ID3D11Device* device, const unsigned char* vertexByteArray, const size_t vertexArraySize);
	bool CreateTextureSampler(ID3D11Device* device);
	bool CreateCBuffers(ID3D11Device* device);
	void RenderShader(ID3D11DeviceContext *deviceContext, int indexCount);

	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCTSTR);

	bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix, Vector cameraPosition, LightBufferType& light, MaterialProperties* mat);
	
	DxObject<ID3D11SamplerState> mSampleState;
	DxObject<ID3D11Buffer> mLightBuffer;
	DxObject<ID3D11Buffer> mCameraBuffer;
	DxObject<ID3D11Buffer> mMaterialBuffer;

	DxObject<ID3D11VertexShader> mVertexShader;
	DxObject<ID3D11PixelShader> mPixelShader;
	DxObject<ID3D11InputLayout> mInputLayout;
	DxObject<ID3D11Buffer> mMatrixBuffer;
};