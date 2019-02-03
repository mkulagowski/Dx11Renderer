#pragma once

class Float2 {
public:
	float f[2];

	Float2(float f1, float f2)
		: f{ f1, f2 }
	{};

	Float2(int f1, int f2)
		: f{ static_cast<float>(f1), static_cast<float>(f2) }
	{};

	Float2()
		: Float2(0.f, 0.f)
	{};
	/*
	Float2(Vector v)
		: Float2(v.f[0], v.f[1])
	{};
	*/
	Float2& operator*= (const float& b)
	{
		f[0] *= b;
		f[1] *= b;
		return *this;
	}
};