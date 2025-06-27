#pragma once
#include "Common/VariableTypes.h"
#include "Common/Input.h"

class DebugCamera
{
private:
	PolarCoordinates pos_;
	Transform camera_;

	Matrix4x4 cameraMatrix;
	Matrix4x4 viewMatrix_;

	

public:

	void Initialize();

	void Update(Input* input);


	Matrix4x4 GetViewMatrix() {return viewMatrix_;}

};

