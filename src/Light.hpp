#pragma once

#include "Vector.hpp"
#include "MathUtils.hpp"
#include <array>
#include "Model.hpp"

constexpr unsigned char LIGHT_NO = 8;

enum LightType
{
	DirectionalLight = 0,
	PointLight = 1,
	SpotLight = 2
};

struct Light
{
	Light()
		: Position(0.0f, 0.0f, 0.0f, 1.0f)
		, Direction(0.0f, 0.0f, 1.0f, 0.0f)
		, Color(1.0f, 1.0f, 1.0f, 1.0f)
		, SpotAngle(PI2)
		, ConstantAttenuation(1.0f)
		, LinearAttenuation(0.0f)
		, QuadraticAttenuation(0.0f)
		, LightType(LightType::DirectionalLight)
		, Enabled(0)
	{}

	Float4    Position;
	Float4 Direction;
	Float4 Color;
	float       SpotAngle;
	float       ConstantAttenuation;
	float       LinearAttenuation;
	float       QuadraticAttenuation;
	int         LightType;
	int         Enabled;
	int         Padding[2];
};

struct LightBufferType
{
	LightBufferType()
		: EyePosition(0.0f, 0.0f, 0.0f, 1.0f)
		, GlobalAmbient(0.2f, 0.2f, 0.8f, 1.0f)
	{}

	Float4 EyePosition;
	Float4 GlobalAmbient;
	Light Lights[LIGHT_NO];
};

class LightManager
{
public:
	LightManager();
	~LightManager();

	std::array<std::unique_ptr<Model>, LIGHT_NO>* GetModels();
	Model* GetModel(int i);
    void RenderUI(ID3D11Device* device);
	Vector GetAmbient() const;
	void SetAmbient(Vector col);

	Light* GetLight(unsigned char i);
	LightBufferType* GetLightBuffer();

	void InitModels(ID3D11Device* device);

private:
	LightBufferType mLightBuffer;
	std::array<std::unique_ptr<Model>, LIGHT_NO> mLightModels;
	std::array<int, LIGHT_NO> mLastType;
};