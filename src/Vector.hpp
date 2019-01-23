#pragma once

#include <cmath>

class alignas(16) Vector {
public:
	static Vector Red() {
		return Vector(1.f, 0.f, 0.f, 1.f);
	};

	static Vector Green() {
		return Vector(0.f, 1.f, 0.f, 1.f);
	};

	static Vector Blue() {
		return Vector(0.f, 0.f, 1.f, 1.f);
	};

	union
	{
		float f[4];
		int i[4];
		unsigned int u[4];
	};


	Vector(float f1, float f2, float f3, float f4)
		: f {f1, f2, f3, f4}
	{}

	Vector(float f1, float f2, float f3)
		: Vector(f1, f2, f3, 0.f)
	{}

	Vector(float f1, float f2)
		: Vector(f1, f2, 0.f, 0.f)
	{}

	Vector(float ff)
		: Vector(ff, ff, ff, ff)
	{}

	Vector()
		: Vector(0.f)
	{}

	static Vector Cross3(const Vector& v1, const Vector& v2)
	{
		Vector vec;
		vec.f[0] = v1.f[1] * v2.f[2] - v1.f[2] * v2.f[1];
		vec.f[1] = v1.f[2] * v2.f[0] - v1.f[0] * v2.f[2];
		vec.f[2] = v1.f[0] * v2.f[1] - v1.f[1] * v2.f[0];
		return vec;
	}

	float Length3() const
	{
		return sqrtf(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
	}

	Vector Normalized3() const
	{
		float lenInv = 1.0f / Length3();
		return Vector(f[0] * lenInv, f[1] * lenInv, f[2] * lenInv, 0.0f);
	}

	static float Dot3(const Vector& v1, const Vector& v2)
	{
		return v1.f[0] * v2.f[0] + v1.f[1] * v2.f[1] + v1.f[2] * v2.f[2];
	}

	Vector SplatX() const
	{
		return Vector(f[0]);
	}

	Vector SplatY() const
	{
		return Vector(f[1]);
	}

	Vector SplatZ() const
	{
		return Vector(f[2]);
	}

	Vector SplatW() const
	{
		return Vector(f[3]);
	}
	static Vector MulAndAdd(const Vector& a, const Vector& b, const Vector& c)
	{
		return a * b + c;
	}
	Vector operator* (const Vector& b) const
	{
		return Vector(*this) *= b;
	}
	Vector& operator*= (const Vector& b)
	{
		f[0] *= b.f[0];
		f[1] *= b.f[1];
		f[2] *= b.f[2];
		f[3] *= b.f[3];
		return *this;
	}
	Vector operator+ (const Vector& b) const
	{
		return Vector(*this) += b;
	}
	Vector& operator+= (const Vector& b)
	{
		f[0] += b.f[0];
		f[1] += b.f[1];
		f[2] += b.f[2];
		f[3] += b.f[3];
		return *this;
	}

	Vector operator- (const Vector& b) const
	{
		return Vector(*this) -= b;
	}

	Vector& operator-= (const Vector& b)
	{
		f[0] -= b.f[0];
		f[1] -= b.f[1];
		f[2] -= b.f[2];
		f[3] -= b.f[3];
		return *this;
	}

};

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

	Float2(Vector v)
		: Float2(v.f[0], v.f[1])
	{};

	Float2& operator*= (const float& b)
	{
		f[0] *= b;
		f[1] *= b;
		return *this;
	}
};

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