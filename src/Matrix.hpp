#pragma once

#include "Vector.hpp"
#include <cmath>

class alignas(16) Matrix
{
public:
	union
	{
		Vector r[4];   //< rows
		float f[16];
		float m[4][4];
	};

	Matrix() {
		r[0] = Vector(1,0,0,0);
		r[1] = Vector(0,1,0,0);
		r[2] = Vector(0,0,1,0);
		r[3] = Vector(0,0,0,1);
	};

	Matrix(Vector v1, Vector v2, Vector v3, Vector v4) {
		r[0] = v1;
		r[1] = v2;
		r[2] = v3;
		r[3] = v4;
	};

	
	static Matrix MakePerspective(float aspect, float fovY, float farZ, float nearZ)
	{
		float yScale = 1.0f / tanf(fovY * 0.5f);
		float xScale = yScale / aspect;

		return Matrix(Vector(xScale, 0.0f, 0.0f, 0.0f),
			Vector(0.0f, yScale, 0.0f, 0.0f),
			Vector(0.0f, 0.0f, farZ / (farZ - nearZ), 1.0f),
			Vector(0.0f, 0.0f, -nearZ * farZ / (farZ - nearZ), 0.0f));
	}

	static Matrix MakeOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		return Matrix(
			Vector(2.0f / (right - left), 0.0f, 0.0f, 0.0f),
			Vector(0.0f, 2.0f / (top - bottom), 0.0f, 0.0f),
			Vector(0.0f, 0.0f, 1.0f / (zFar - zNear), 0.0f),
			Vector((left + right) / (left - right),
			(top + bottom) / (bottom - top),
				zNear / (zNear - zFar),
				1.0f));
	}

	static Matrix MakeLookTo(const Vector& EyePosition, const Vector& EyeDirection,
		const Vector& UpDirection)
	{
		Vector zaxis = EyeDirection.Normalized3();
		Vector xaxis = Vector::Cross3(UpDirection, zaxis).Normalized3();
		Vector yaxis = Vector::Cross3(zaxis, xaxis);

		return Matrix(Vector(xaxis.f[0], yaxis.f[0], zaxis.f[0], 0.0f),
			Vector(xaxis.f[1], yaxis.f[1], zaxis.f[1], 0.0f),
			Vector(xaxis.f[2], yaxis.f[2], zaxis.f[2], 0.0f),
			Vector(-Vector::Dot3(xaxis, EyePosition),
				-Vector::Dot3(yaxis, EyePosition),
				-Vector::Dot3(zaxis, EyePosition),
				1.0f));
	}

	static Matrix MakeLookAt(const Vector& EyePosition, const Vector& FocusPosition,
		const Vector& UpDirection)
	{
		return MakeLookTo(EyePosition, FocusPosition - EyePosition, UpDirection);
	}

	static Matrix MakeRotation(float yaw, float pitch, float roll)
	{
		Matrix mx(
			Vector(1.f, 0.f, 0.f, 0.f),
			Vector(0.f, cos(yaw), -sin(yaw), 0.f),
			Vector(0.f, sin(yaw), cos(yaw), 0.f),
			Vector(0.f, 0.f, 0.f, 0.f)
		);
		Matrix my(
			Vector(cos(pitch), 0.f, sin(pitch), 0.f),
			Vector(0.f, 1.f, 0.f, 0.f),
			Vector(-sin(pitch), 0.f, cos(pitch), 0.f),
			Vector(0.f, 0.f, 0.f, 0.f)
		);
		Matrix mz(
			Vector(cos(roll), -sin(roll), 0.f, 0.f),
			Vector(sin(roll), cos(roll), 0.f, 0.f),
			Vector(0.f, 0.f, 1.f, 0.f),
			Vector(0.f, 0.f, 0.f, 0.f)
		);

		return (mz * my) * mx;
	}

	static Matrix MakeRotationNormal(const Vector& normalAxis, float angle)
	{
		Matrix result;

		float x = normalAxis.f[0];
		float y = normalAxis.f[1];
		float z = normalAxis.f[2];
		float c = cosf(angle);
		float s = sinf(angle);
		float t = 1.0f - c;

		result.r[0] = Vector(t * x * x + c,
			t * x * y - s * z,
			t * x *z + s * y,
			0.0f);

		result.r[1] = Vector(t * x * y + s * z,
			t * y * y + c,
			t * y * z - s * x,
			0.0f);

		result.r[2] = Vector(t * x * z - s * y,
			t * y * z + s * x,
			t * z * z + c,
			0.0f);

		result.r[3] = Vector(0.f, 0.f, 0.f, 1.f);

		return result;
	}

	Vector LinearCombination3(const Vector& a) const
	{
		const Vector tmp0 = Vector::MulAndAdd(a.SplatX(), r[0], a.SplatY() * r[1]);
		const Vector tmp1 = Vector::MulAndAdd(a.SplatZ(), r[2], r[3]);
		return tmp0 + tmp1;
	}

	Vector LinearCombination4(const Vector& a) const
	{
		const Vector tmp0 = Vector::MulAndAdd(a.SplatX(), r[0], a.SplatY() * r[1]);
		const Vector tmp1 = Vector::MulAndAdd(a.SplatZ(), r[2], a.SplatW() * r[3]);
		Vector res = tmp0 + tmp1;
		res.f[3] = 1;
		return res;
	}

	Matrix operator* (const Matrix& b) const
	{
		return Matrix(b.LinearCombination4(r[0]),
			b.LinearCombination4(r[1]),
			b.LinearCombination4(r[2]),
			b.LinearCombination4(r[3]));
	}

	Matrix Transposed() const
	{
		Matrix mat;
		mat.r[0] = Vector(r[0].f[0], r[1].f[0], r[2].f[0], r[3].f[0]);
		mat.r[1] = Vector(r[0].f[1], r[1].f[1], r[2].f[1], r[3].f[1]);
		mat.r[2] = Vector(r[0].f[2], r[1].f[2], r[2].f[2], r[3].f[2]);
		mat.r[3] = Vector(r[0].f[3], r[1].f[3], r[2].f[3], r[3].f[3]);
		return mat;
	}

	static Matrix MakeTranslation3(const Vector& pos)
	{
		Matrix m;
		m.r[0] = Vector(1, 0, 0, 0);
		m.r[1] = Vector(0, 1, 0, 0);
		m.r[2] = Vector(0, 0, 1, 0);
		m.r[3] = Vector(pos.f[0], pos.f[1], pos.f[2], 1.0f);
		return m;
	}

	Matrix Translate(const Vector& pos)
	{
		Matrix m;
		m.r[0] = Vector(1, 0, 0, 0);
		m.r[1] = Vector(0, 1, 0, 0);
		m.r[2] = Vector(0, 0, 1, 0);
		m.r[3] = Vector(pos.f[0], pos.f[1], pos.f[2], 1.0f);
		return m * *this;
	}
};