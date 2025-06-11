#pragma once
#include "Common/VariableTypes.h"

class Camera
{
private:
	int kClientWidth = 1280;
	int kClientHeight = 720;
	float fovY = 0.45f;
	float zn = 0.1f;
	float zf = 100.0f;

public:

	Matrix4x4 MakeWorldViewProjectionMatrix(Transform transform, Transform camera);


};

