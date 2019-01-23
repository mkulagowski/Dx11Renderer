#pragma once

#include "Vector.hpp"

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
};