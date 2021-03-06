#include "Model.hpp"

#include "model_obj/model_obj.h"
#include "Utils/IcosphereCreator.hpp"
#include "MathUtils.hpp"
#include "Logger.hpp"

Model::Model()
{
}

Model::~Model()
{
}

bool Model::Init(ID3D11Device * device)
{
	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	return InitializeBuffers(device);
}

bool Model::Init(ID3D11Device * device, std::shared_ptr<MaterialProperties> material)
{
	if (!Init(device))
		return false;

	mMaterial = material;
	return true;
}

void Model::SetMaterial(std::shared_ptr<MaterialProperties> material)
{
	mMaterial = material;
}

MaterialProperties * Model::GetMaterial() const
{
	return mMaterial.get();
}

void Model::LoadCube(Vector halfSize)
{
	mVertices.clear();
	mVertices.reserve(4 * 6);

	mVertices.push_back({ Float3(-1,  1, -1) , Float2(0, 0) , Float3( 0,  0, -1) });
	mVertices.push_back({ Float3( 1,  1, -1) , Float2(1, 0) , Float3( 0,  0, -1) });
	mVertices.push_back({ Float3(-1, -1, -1) , Float2(0, 1) , Float3( 0,  0, -1) });
	mVertices.push_back({ Float3( 1, -1, -1) , Float2(1, 1) , Float3( 0,  0, -1) });

	mVertices.push_back({ Float3( 1,  1, -1) , Float2(0, 0) , Float3( 1,  0,  0) });
	mVertices.push_back({ Float3( 1,  1,  1) , Float2(1, 0) , Float3( 1,  0,  0) });
	mVertices.push_back({ Float3( 1, -1, -1) , Float2(0, 1) , Float3( 1,  0,  0) });
	mVertices.push_back({ Float3( 1, -1,  1) , Float2(1, 1) , Float3( 1,  0,  0) });

	mVertices.push_back({ Float3( 1,  1,  1) , Float2(0, 0) , Float3( 0,  0,  1) });
	mVertices.push_back({ Float3(-1,  1,  1) , Float2(1, 0) , Float3( 0,  0,  1) });
	mVertices.push_back({ Float3( 1, -1,  1) , Float2(0, 1) , Float3( 0,  0,  1) });
	mVertices.push_back({ Float3(-1, -1,  1) , Float2(1, 1) , Float3( 0,  0,  1) });

	mVertices.push_back({ Float3(-1,  1,  1) , Float2(0, 0) , Float3(-1,  0,  0) });
	mVertices.push_back({ Float3(-1,  1, -1) , Float2(1, 0) , Float3(-1,  0,  0) });
	mVertices.push_back({ Float3(-1, -1,  1) , Float2(0, 1) , Float3(-1,  0,  0) });
	mVertices.push_back({ Float3(-1, -1, -1) , Float2(1, 1) , Float3(-1,  0,  0) });

	mVertices.push_back({ Float3(-1,  1,  1) , Float2(0, 0) , Float3( 0,  1,  0) });
	mVertices.push_back({ Float3( 1,  1,  1) , Float2(1, 0) , Float3( 0,  1,  0) });
	mVertices.push_back({ Float3(-1,  1, -1) , Float2(0, 1) , Float3( 0,  1,  0) });
	mVertices.push_back({ Float3( 1,  1, -1) , Float2(1, 1) , Float3( 0,  1,  0) });

	mVertices.push_back({ Float3(-1, -1, -1) , Float2(0, 0) , Float3( 0, -1,  0) });
	mVertices.push_back({ Float3( 1, -1, -1) , Float2(1, 0) , Float3( 0, -1,  0) });
	mVertices.push_back({ Float3(-1, -1,  1) , Float2(0, 1) , Float3( 0, -1,  0) });
	mVertices.push_back({ Float3( 1, -1,  1) , Float2(1, 1) , Float3( 0, -1,  0) });

	for (auto &i : mVertices)
		i.position = (Vector(i.position.f[0], i.position.f[1], i.position.f[2]) * halfSize).ToFloat3();


	mIndices.clear();
	mIndices.reserve(6 * 6);
	// Load the index array with data.
	mIndices.push_back(0);  // Bottom left.
	mIndices.push_back(1);  // Top middle.
	mIndices.push_back(2);  // Bottom right.
	mIndices.push_back(2);  // Bottom left.
	mIndices.push_back(1);  // Top middle.
	mIndices.push_back(3);  // Bottom right.

	mIndices.push_back(4);  // Bottom left.
	mIndices.push_back(5);  // Top middle.
	mIndices.push_back(6);  // Bottom right.
	mIndices.push_back(6);  // Bottom left.
	mIndices.push_back(5);  // Top middle.
	mIndices.push_back(7);  // Bottom right.

	mIndices.push_back(8);  // Bottom left.
	mIndices.push_back(9);  // Top middle.
	mIndices.push_back(10);  // Bottom right.
	mIndices.push_back(10);  // Bottom left.
	mIndices.push_back(9);  // Top middle.
	mIndices.push_back(11);  // Bottom right.

	mIndices.push_back(12);  // Bottom left.
	mIndices.push_back(13);  // Top middle.
	mIndices.push_back(14);  // Bottom right.
	mIndices.push_back(14);  // Bottom left.
	mIndices.push_back(13);  // Top middle.
	mIndices.push_back(15);  // Bottom right.

	mIndices.push_back(16);  // Bottom left.
	mIndices.push_back(17);  // Top middle.
	mIndices.push_back(18);  // Bottom right.
	mIndices.push_back(18);  // Bottom left.
	mIndices.push_back(17);  // Top middle.
	mIndices.push_back(19);  // Bottom right.

	mIndices.push_back(20);  // Bottom left.
	mIndices.push_back(21);  // Top middle.
	mIndices.push_back(22);  // Bottom right.
	mIndices.push_back(22);  // Bottom left.
	mIndices.push_back(21);  // Top middle.
	mIndices.push_back(23);  // Bottom right.
}

void Model::LoadIcoSphere(float radius, uint8_t subdivisions)
{
	IcosphereCreator creator;
	creator.Create(subdivisions);
	mVertices.clear();
	mVertices.reserve(creator.GetVerticesNumber());
	for (auto &i : creator.GetVertices())
	{
		Vector normal = i.Normalized3();
		mVertices.push_back({
			(i * radius).ToFloat3(),
			Float2(asinf(normal.f[0]) / PI + 0.5f, asinf(normal.f[1]) / PI + 0.5f),
			normal.ToFloat3()
			});
	}

	mIndices.clear();
	mIndices.reserve(creator.GetIndicesNumber());
	for (auto &i : creator.GetIndices())
	{
		mIndices.push_back(static_cast<uint32_t>(i));
	}
}

void Model::LoadPyramid(Vector halfSize)
{
	mVertices.clear();
	mVertices.reserve(16);

	const float x = halfSize.f[0];
	const float y = halfSize.f[1];
	const float z = halfSize.f[2];

	auto simpleNormal = [&](Vector v1, Vector v2, Vector v3)
	{
		Vector edge1 = v2 - v1;
		Vector edge2 = v3 - v1;
		return Vector::Cross3(edge1, edge2).Normalized3().ToFloat3();
	};

	Float3 simpleNormB(simpleNormal(Vector( x, -y,  z), Vector(-x, -y,  z), Vector(0, y, 0)));
	Float3 simpleNormL(simpleNormal(Vector(-x, -y,  z), Vector(-x, -y, -z), Vector(0, y, 0)));
	Float3 simpleNormF(simpleNormal(Vector(-x, -y, -z), Vector( x, -y, -z), Vector(0, y, 0)));
	Float3 simpleNormR(simpleNormal(Vector( x, -y,  z), Vector( x, -y,  z), Vector(0, y, 0)));


	mVertices.push_back({ Float3(0.f,  y, 0.f) , Float2(.0f, .5f) , simpleNormL }); //< top
	mVertices.push_back({ Float3(-x,  -y, z) , Float2(1.0f, .0f) , simpleNormL }); //< left
	mVertices.push_back({ Float3(-x,  -y, -z) , Float2(1.0f, 1.f) , simpleNormL }); //< left

	mVertices.push_back({ Float3(0.f,  y, 0.f) , Float2(.0f, .5f) , simpleNormF }); //< top
	mVertices.push_back({ Float3(-x,  -y, -z) , Float2(1.0f, .0f) , simpleNormF }); //< front
	mVertices.push_back({ Float3(x,  -y, -z) , Float2(1.0f, 1.f) , simpleNormF }); //< front

	mVertices.push_back({ Float3(0.f,  y, 0.f) , Float2(.0f, .5f) , simpleNormR }); //< top
	mVertices.push_back({ Float3(x,  -y, -z) , Float2(1.0f, .0f) , simpleNormR }); //< right
	mVertices.push_back({ Float3(x,  -y, z) , Float2(1.0f, 1.f) , simpleNormR }); //< right

	mVertices.push_back({ Float3(0.f,  y, 0.f) , Float2(.0f, .5f) , simpleNormB }); //< top
	mVertices.push_back({ Float3(x,  -y, z) , Float2(1.0f, .0f) , simpleNormB }); //< back
	mVertices.push_back({ Float3(-x,  -y, z) , Float2(1.0f, 1.f) , simpleNormB }); //< back

	mVertices.push_back({ Float3(-x,  -y, z) , Float2(.0f, 1.0f) , Float3(0, -1, 0) }); //< LB 9
	mVertices.push_back({ Float3(x,  -y, z) , Float2(1.0f, 1.f) , Float3(0, -1, 0) }); //< RB 10
	mVertices.push_back({ Float3(x,  -y, -z) , Float2(1.0f, .0f) , Float3(0, -1, 0) }); //< RF 11
	mVertices.push_back({ Float3(-x,  -y, -z) , Float2(.0f, .0f) , Float3(0, -1, 0) }); //< LF 12

	mIndices.clear();
	mIndices.reserve(6 * 3);
	// Load the index array with data.
	mIndices.push_back(12);
	mIndices.push_back(15);
	mIndices.push_back(13);
	mIndices.push_back(15);
	mIndices.push_back(14);
	mIndices.push_back(13);

	mIndices.push_back(0);
	mIndices.push_back(2);
	mIndices.push_back(1);

	mIndices.push_back(3);
	mIndices.push_back(5);
	mIndices.push_back(4);

	mIndices.push_back(6);
	mIndices.push_back(8);
	mIndices.push_back(7);

	mIndices.push_back(9);
	mIndices.push_back(11);
	mIndices.push_back(10);
}

void Model::LoadPlane(float halfWidth, float halfHeight)
{
	mVertices.clear();
	mVertices.reserve(4);

	mVertices.push_back({ Float3(-halfWidth, 0.f, -halfHeight) , Float2(0, 1) , Float3(0, 1, 0) });
	mVertices.push_back({ Float3(-halfWidth, 0.f,  halfHeight) , Float2(0, 0) , Float3(0, 1, 0) });
	mVertices.push_back({ Float3( halfWidth, 0.f, -halfHeight) , Float2(1, 1) , Float3(0, 1, 0) });
	mVertices.push_back({ Float3( halfWidth, 0.f,  halfHeight) , Float2(1, 0) , Float3(0, 1, 0) });

	mIndices.clear();
	mIndices.reserve(6);
	// Load the index array with data.
	mIndices.push_back(0);  // Bottom left.
	mIndices.push_back(1);  // Top middle.
	mIndices.push_back(2);  // Bottom right.
	mIndices.push_back(2);  // Bottom left.
	mIndices.push_back(1);  // Top middle.
	mIndices.push_back(3);  // Bottom right.
}

std::vector<std::unique_ptr<Model>> Model::LoadFromObjFile(std::string path, ID3D11Device* dev)
{
	LOGI("Loading model from \"%s\"", path.c_str());
	ModelOBJ mdl;
	mdl.import(path.c_str(), false);
	int meshNo = mdl.getNumberOfMeshes();
	if (meshNo == 0) {
		LOGW("No meshes found!");
		return std::vector<std::unique_ptr<Model>>();
	}
	LOGI("%d meshes loaded!", meshNo);
	uint64_t sizeV = mdl.getNumberOfVertices();
	uint64_t sizeI = mdl.getNumberOfIndices();


	std::vector<std::unique_ptr<Model>> models;
	models.reserve(meshNo);
	const std::string dirPath = mdl.getPath();
	std::unordered_map<std::string, std::shared_ptr<MaterialProperties>> name2MatPtr;
	
	int matNo = mdl.getNumberOfMaterials();
	LOGI("Loading %d materials...", matNo);
	for (int i = 0; i < matNo; ++i)
	{
		auto mat = mdl.getMaterial(i);

		std::shared_ptr<MaterialProperties> meshMaterial = std::make_shared<MaterialProperties>();
		meshMaterial->mMaterial.mUseTexture = true;
		meshMaterial->mTexture = std::make_shared<Texture>();
		LOGI("\t %s from \"%s\"", mat.name.c_str(), mat.colorMapFilename.c_str());
		meshMaterial->mTexture->Initialize(dev, dirPath + mat.colorMapFilename);

		meshMaterial->mMaterial.mAmbient = mat.ambient;
		meshMaterial->mMaterial.mDiffuse = mat.diffuse;
		meshMaterial->mMaterial.mSpecular = mat.specular;
		meshMaterial->mMaterial.mSpecularPower = mat.shininess;

		name2MatPtr.insert( { mat.name, meshMaterial } );
	}

	const ModelOBJ::Vertex* ptr = mdl.getVertexBuffer();
	const int* ptri = mdl.getIndexBuffer();

	for (int i = 0; i < meshNo; ++i)
	{
		auto mesh = mdl.getMesh(i);
		models.push_back(std::make_unique<Model>());
		models.back()->mMaterial = name2MatPtr[mesh.pMaterial->name];
		models.back()->tag = "[" + std::to_string(i) + "] - " + mesh.pMaterial->name;
		models.back()->mVertices.reserve(mesh.triangleCount * 3);
		models.back()->mIndices.reserve(mesh.triangleCount * 3);

		for (int j = 0; j < mesh.triangleCount * 3; j++) {
			uint32_t vertIdx = static_cast<uint32_t>(ptri[mesh.startIndex + j]);

			models.back()->mVertices.push_back({
				Float3(ptr[vertIdx].position[0], ptr[vertIdx].position[1], ptr[vertIdx].position[2]),
				Float2(ptr[vertIdx].texCoord[0], 1 - ptr[vertIdx].texCoord[1]),
				Float3(ptr[vertIdx].normal[0]  , ptr[vertIdx].normal[1]  , ptr[vertIdx].normal[2])
				});
			models.back()->mIndices.push_back(j);
		}
	}

	return models;
}

void Model::Bind(ID3D11DeviceContext *deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(ModelVertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.getAt(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(mIndexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int Model::GetIndexCount() const
{
	return static_cast<int>(mIndices.size());
}

Vector Model::GetPosition() const
{
	return mPosition;
}

void Model::SetPosition(const Vector & pos)
{
	mPosition = pos;
}

Matrix Model::GetRotation() const
{
	return mRotation;
}

void Model::SetRotation(const Matrix & rot)
{
	mRotation = rot;
}

Matrix Model::GetWorldMatrix() const
{
	return mRotation * Matrix::MakeTranslation3(mPosition);
}

bool Model::InitializeBuffers(ID3D11Device * device)
{
	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(decltype (mVertices)::value_type) * mVertices.size());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = mVertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, mVertexBuffer.getAt());
	if (FAILED(result))
	{
		LOGE("CreateBuffer for vertex failed!");
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(decltype (mIndices)::value_type) * mIndices.size());
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = mIndices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, mIndexBuffer.getAt());

	if (FAILED(result))
	{
		LOGE("CreateBuffer for index failed!");
		return false;
	}

	return true;
}