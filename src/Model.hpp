#pragma once

#include <d3d11.h>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "DxObject.hpp"
#include <vector>
#include "Float2.hpp"
#include "Float3.hpp"

class Model
{
public:
	struct ModelVertexType
	{
		Float3 position;
		Float2 tex;
		Float3 normal;
	};

	Model();
	~Model();

	bool Init(ID3D11Device* device);
	bool Init(ID3D11Device* device, std::string texturePath);
	bool Init(ID3D11Device* device, std::shared_ptr<Texture> texture);
	void LoadCube(Vector halfSize);
	void LoadIcoSphere(float radius, uint8_t subdivisions);
	void LoadCone(float halfHeight, float radius, uint8_t subdivisions);
	void LoadPyramid(Vector halfSize);
	void LoadPlane(float halfWidth, float halfHeight);
	void LoadFromObjFile(std::string path);
	void Bind(ID3D11DeviceContext* ctx);

	Texture* GetTexture();
	bool HasTexture() const;
	void SetTexture(std::shared_ptr<Texture> tex);
	int GetIndexCount() const;
	Vector GetPosition() const;
	void SetPosition(const Vector &pos);

	Matrix mRot;
	std::shared_ptr<MaterialProperties> mMat;
private:

	bool InitializeBuffers(ID3D11Device*);

	DxObject<ID3D11Buffer> mVertexBuffer, mIndexBuffer;
	std::shared_ptr<Texture> mTexture;

	Vector mPosition;
	std::vector<ModelVertexType> mVertices;
	std::vector<uint32_t> mIndices;
};