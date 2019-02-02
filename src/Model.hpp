#pragma once

#include <d3d11.h>
#include "Vector.hpp"
#include "Matrix.hpp"
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
	bool Init(ID3D11Device* device, std::shared_ptr<MaterialProperties> material);
	
	void LoadCube(Vector halfSize);
	void LoadIcoSphere(float radius, uint8_t subdivisions);
	void LoadCone(float halfHeight, float radius, uint8_t subdivisions);
	void LoadPyramid(Vector halfSize);
	void LoadPlane(float halfWidth, float halfHeight);
	void LoadFromObjFile(std::string path);

	void SetMaterial(std::shared_ptr<MaterialProperties> material);
	MaterialProperties* GetMaterial() const;
	
	Vector GetPosition() const;
	void SetPosition(const Vector &pos);

	Matrix GetRotation() const;
	void SetRotation(const Matrix &rot);

	Matrix GetWorldMatrix() const;

	void Bind(ID3D11DeviceContext* ctx);
	int GetIndexCount() const;
	
	
private:
	Vector mPosition;
	Matrix mRotation;
	std::shared_ptr<MaterialProperties> mMaterial;
	DxObject<ID3D11Buffer> mVertexBuffer, mIndexBuffer;
	std::vector<ModelVertexType> mVertices;
	std::vector<uint32_t> mIndices;

	bool InitializeBuffers(ID3D11Device* device);
};