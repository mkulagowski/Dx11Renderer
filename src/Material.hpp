#pragma once

#include "Float4.hpp"
#include "Texture.hpp"
#include <memory>

struct Material
{
	Material()
		: mEmissive(0.0f, 0.0f, 0.0f, 1.0f)
		, mAmbient(0.1f, 0.1f, 0.1f, 1.0f)
		, mDiffuse(1.0f, 1.0f, 1.0f, 1.0f)
		, mSpecular(1.0f, 1.0f, 1.0f, 1.0f)
		, mSpecularPower(128.0f)
		, mUseTexture(false)
	{}

	Float4 mEmissive;
	Float4 mAmbient;
	Float4 mDiffuse;
	Float4 mSpecular;
	float mSpecularPower;
	int mUseTexture;
	float _Padding[2];
};

struct MaterialProperties
{
	Material mMaterial;
	std::shared_ptr<Texture> mTexture;
};