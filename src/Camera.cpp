#include "Camera.hpp"
#include "MathUtils.hpp"

Camera::Camera()
	: mPosition(0.f)
	, mRotation(0.f)
	, mHAngle(PI)
	, mVAngle(0.f)
	, mInitialFoV(45.f)
{
}

Camera::Camera(Vector pos, Vector rot)
	: mPosition(pos)
	, mRotation(rot)
	, mHAngle(0.f)
	, mVAngle(0.f)
	, mInitialFoV(45.f)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = Vector(x, y, z);
}

void Camera::SetPosition(Vector pos)
{
	mPosition = pos;
}

void Camera::SetRotation(float x, float y, float z)
{
	mRotation = Vector(x, y, z);
}

void Camera::SetRotation(Vector rot)
{
	mRotation = rot;
}

Vector Camera::GetPosition()
{
	return mPosition;
}

Vector Camera::GetRotation()
{
	return mRotation;
}
//#include <cstdio>
void Camera::MoveCamera(float x, float y, double dTime, Camera::Move moveDir)
{
	static auto speed = 200.f;
	static auto mouseSpeed = 0.05f;
	mHAngle += static_cast<float>(x * mouseSpeed * dTime);
	mVAngle -= static_cast<float>(y * mouseSpeed * dTime);

	//printf("MOUSEX: %f, MOUSEY: %f\n", x, y);
	//printf("ANGLEX: %f, ANGLEY: %f\n", mHAngle, mVAngle);
	Vector dir(
		cos(mVAngle) * sin(mHAngle),
		sin(mVAngle),
		cos(mVAngle) * cos(mHAngle)
	);
	Vector right(
		sin(mHAngle - PI2),
		0,
		cos(mHAngle - PI2)
	);

	Vector up = Vector::Cross3(right, dir);

	switch (moveDir)
	{
	case Camera::Move::Fwd:
		mPosition += dir * static_cast<float>(dTime * speed);
		break;

	case Camera::Move::Bwd:
		mPosition -= dir * static_cast<float>(dTime * speed);
		break;

	case Camera::Move::Rt:
		mPosition -= right * static_cast<float>(dTime * speed);
		break;

	case Camera::Move::Lt:
		mPosition += right * static_cast<float>(dTime * speed);
		break;

	default:
		break;
	}
	mViewMatrix = Matrix::MakeLookTo(mPosition, dir, up);
}

void Camera::RecalculateMatrix()
{
	// Setup the vector that points upwards.
	Vector up(0.f, 1.f, 0.f, 0.f);

	Vector lookAt(0.f, 0.f, 1.f, 0.f);
	// Setup where the camera is looking by default.

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	float pitch = mRotation.f[0] * 0.0174532925f;
	float yaw	= mRotation.f[1] * 0.0174532925f;
	float roll	= mRotation.f[2] * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	Matrix rotationMatrix = Quaternion::FromAngles(pitch, yaw, roll).ToMatrix();

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = rotationMatrix.LinearCombination4(lookAt);
	up = rotationMatrix.LinearCombination4(up);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = mPosition + lookAt;

	// Finally create the view matrix from the three updated vectors.
	mViewMatrix = Matrix::MakeLookAt(mPosition, lookAt, up);
}

Matrix Camera::GetViewMatrix() const
{
	return mViewMatrix;
}
