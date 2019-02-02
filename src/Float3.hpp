#pragma once

#include "Vector.hpp"

class Float3 {
public:
	float f[3];

	Float3(float f1, float f2, float f3)
		: f{ f1, f2, f3 }
	{};

	Float3(int f1, int f2, int f3)
		: f{ static_cast<float>(f1), static_cast<float>(f2), static_cast<float>(f3) }
	{};

	Float3()
		: Float3(0.f, 0.f, 0.f)
	{};

	Float3(Vector v)
		: Float3(v.f[0], v.f[1], v.f[2])
	{};

	Float3& operator*= (const float& b)
	{
		f[0] *= b;
		f[1] *= b;
		f[2] *= b;
		return *this;
	}
};
