#pragma once
#include "Matrix.hpp"
#include "Vector.hpp"

class alignas(16) Quaternion
{
public:
	// XYZ - vector part, W - scalar part:
	// q = f[3] + i * q[0] + j * q[1] + k * q[2]
	union
	{
		Vector q;
		float f[4];
	};

	Quaternion()
		: q(0.0f, 0.0f, 0.0f, 1.0f)
	{}

	Quaternion(const Quaternion&) = default;

	Quaternion(const Vector& v)
		: q(v)
	{ }

	Quaternion(float i, float j, float k, float s)
		: q(i, j, k, s)
	{}

	operator const Vector&() const { return q; }
	operator Vector&() { return q; }

	static Quaternion FromAngles(float pitch, float yaw, float roll)
	{
		// based on: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		pitch *= 0.5f;
		yaw *= 0.5f;
		roll *= 0.5f;

		Quaternion q;
		float t0 = cosf(yaw);
		float t1 = sinf(yaw);
		float t2 = cosf(roll);
		float t3 = sinf(roll);
		float t4 = cosf(pitch);
		float t5 = sinf(pitch);

		const Vector term0 = Vector(t0, t1, t0, t0);
		const Vector term1 = Vector(t2, t2, t3, t2);
		const Vector term2 = Vector(t5, t4, t4, t4);

		const Vector term3 = Vector(t1, -t0, -t1, t1);
		const Vector term4 = Vector(t3, t3, t2, t3);
		const Vector term5 = Vector(t4, t5, t5, t5);

		return term0 * term1 * term2 + term3 * term4 * term5;
	}

	Matrix ToMatrix() const
	{
		float xx = q.f[0] * q.f[0], yy = q.f[1] * q.f[1], zz = q.f[2] * q.f[2];
		float xy = q.f[0] * q.f[1], xz = q.f[0] * q.f[2];
		float yz = q.f[1] * q.f[2], wx = q.f[3] * q.f[0];
		float wy = q.f[3] * q.f[1], wz = q.f[3] * q.f[2];

		Matrix m;
		m.m[0][0] = 1.0f - 2.0f * (yy + zz);
		m.m[0][1] = 2.0f * (xy + wz);
		m.m[0][2] = 2.0f * (xz - wy);
		m.m[0][3] = 0.0f;

		m.m[1][0] = 2.0f * (xy - wz);
		m.m[1][1] = 1.0f - 2.0f * (xx + zz);
		m.m[1][2] = 2.0f * (yz + wx);
		m.m[1][3] = 0.0f;

		m.m[2][0] = 2.0f * (xz + wy);
		m.m[2][1] = 2.0f * (yz - wx);
		m.m[2][2] = 1.0f - 2.0f * (xx + yy);
		m.m[2][3] = 0.0f;

		m.m[3][0] = 0.0f;
		m.m[3][1] = 0.0f;
		m.m[3][2] = 0.0f;
		m.m[3][3] = 1.0f;

		return m;
	}

};