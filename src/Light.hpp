#pragma once

#include "Vector.hpp"

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
		, SpotAngle(3.1415f / 2.f)
		, ConstantAttenuation(1.0f)
		, LinearAttenuation(0.0f)
		, QuadraticAttenuation(0.0f)
		, LightType(LightType::DirectionalLight)
		, Enabled(0)
	{}

	Float4    Position;
	//----------------------------------- (16 byte boundary)
	Float4 Direction;
	//----------------------------------- (16 byte boundary)
	Float4 Color;
	//----------------------------------- (16 byte boundary)
	float       SpotAngle;
	float       ConstantAttenuation;
	float       LinearAttenuation;
	float       QuadraticAttenuation;
	//----------------------------------- (16 byte boundary)
	int         LightType;
	int         Enabled;
	// Add some padding to make this struct size a multiple of 16 bytes.
	int         Padding[2];
	//----------------------------------- (16 byte boundary)
};

struct LightBufferType
{
	LightBufferType()
		: EyePosition(0.0f, 0.0f, 0.0f, 1.0f)
		, GlobalAmbient(0.2f, 0.2f, 0.8f, 1.0f)
	{}

	Float4 EyePosition;
	Float4 GlobalAmbient;
	Light Lights[8];

	/*
	Float4 ambientColor;
	Float4 diffuseColor;
	Float3 lightDirection;
	float specularPower;
	Float4 specularColor;*/
};

/*

class Light
{
public:
	Light();
	~Light();

	void SetDiffuseColor(float, float, float, float);
	void SetAmbient(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float r, float g, float b, float a);
	void SetSpecularPower(float power);

	Vector GetDiffuseColor();
	Vector GetDirection();
	Vector GetAmbient();
	Vector GetSpecularColor();
	float GetSpecularPower();

private:
	Vector m_diffuseColor;
	Vector m_direction;
	Vector mAmbient;
	Vector mSpecular;
	float mSpecularPower;
};*/