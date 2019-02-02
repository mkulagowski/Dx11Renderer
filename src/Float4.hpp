#pragma once

#include "Vector.hpp"

class Float4 {
public:
	float f[4];

	Float4(float f1, float f2, float f3, float f4)
		: f{ f1, f2, f3, f4 }
	{};

	Float4(int f1, int f2, int f3, int f4)
		: f{ static_cast<float>(f1), static_cast<float>(f2), static_cast<float>(f3), static_cast<float>(f4) }
	{};

	Float4()
		: Float4(0.f, 0.f, 0.f, 0.f)
	{};

	Float4(Vector v)
		: Float4(v.f[0], v.f[1], v.f[2], v.f[3])
	{};
};