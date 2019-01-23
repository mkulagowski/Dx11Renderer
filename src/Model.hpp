#pragma once

#include <d3d11.h>
#include "Vector.hpp"
#include "Texture.hpp"
#include "DxObject.hpp"
#include <vector>

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
	void LoadCube(float halfSize);
	void LoadIcoSphere(float radius, uint8_t subdivisions);
	void LoadSquare();
	void LoadFromObjFile(std::string path);
	void Bind(ID3D11DeviceContext* ctx);

	Texture* GetTexture();
	void SetTexture(std::shared_ptr<Texture> tex);
	int GetIndexCount() const;
	Vector GetPosition() const;
	void SetPosition(const Vector &pos);

private:

	bool InitializeBuffers(ID3D11Device*);

	DxObject<ID3D11Buffer> mVertexBuffer, mIndexBuffer;
	std::shared_ptr<Texture> mTexture;

	Vector mPosition;
	std::vector<ModelVertexType> mVertices;
	std::vector<uint32_t> mIndices;
};