#pragma once

#include <cmath>
#include "Float3.hpp"
#include "Float4.hpp"

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

	Vector(Float4 ff)
		: Vector(ff.f[0], ff.f[1], ff.f[2], ff.f[3])
	{}

	Vector(Float3 ff)
		: Vector(ff.f[0], ff.f[1], ff.f[2])
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

	Vector operator/= (float b)
	{
		f[0] /= b;
		f[1] /= b;
		f[2] /= b;
		f[3] /= b;
		return *this;
	}

	Vector operator/ (float b) const
	{
		return Vector(*this) /= b;
	}

	Vector& operator-= (const Vector& b)
	{
		f[0] -= b.f[0];
		f[1] -= b.f[1];
		f[2] -= b.f[2];
		f[3] -= b.f[3];
		return *this;
	}

	Float4 ToFloat4() const
	{
		return Float4(f[0], f[1], f[2], f[3]);
	}

	Float3 ToFloat3() const
	{
		return Float3(f[0], f[1], f[2]);
	}
};
