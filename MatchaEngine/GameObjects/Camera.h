#pragma once
#include "../Core/VariableTypes.h"
#include "DebugCamera.h"


class Camera
{
private:

	Transform transform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Matrix4x4 viewMatrix_{};

	DebugCamera debugCamera_;

	bool isDebugCamera_=true;

public:

	void ImGui();

	void Initialize();
	void Update();
	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }

	void SetTransform(Transform transform) { transform_ = transform; }

};

