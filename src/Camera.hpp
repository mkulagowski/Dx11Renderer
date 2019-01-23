#pragma once

#include "Matrix.hpp"
#include "Quaternion.hpp"

class Camera
{
public:
	enum class Move {
		Lt, 
		Rt, 
		Fwd, 
		Bwd,
		None
	};

	Camera();
	Camera(Vector pos, Vector rot);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetPosition(Vector pos);
	void SetRotation(float x, float y, float z);
	void SetRotation(Vector rot);

	Vector GetPosition();
	Vector GetRotation();
	
	void MoveCamera(float x, float y, double dTime, Camera::Move moveDir);
	void RecalculateMatrix();
	Matrix GetViewMatrix() const;

private:
	float mHAngle;
	float mVAngle;
	float mInitialFoV;
	Vector mPosition;
	Vector mRotation;
	Matrix mViewMatrix;

};