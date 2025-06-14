#pragma once
#include "MatchaEngine/Common/VariableTypes.h"
#include "Input.h"

class DebugCamera
{
private:

	Vector3 translation_ = { 0,0,-50 };
	Vector3 rotation_ = { 0,0,0 };

	Matrix4x4 viewMatrix_;//ビュー行列
	Matrix4x4 ProjectionMatrix_;//射影行列

	float fovY = 0.45f;
	float aspect = 1280.0f / 720.0f;
	float zn = 0.1f;
	float zf = 100.0f;

	Transform camera_ ;
	Matrix4x4 cameraMatrix_;

	Matrix4x4 matRot_;

	float speed = 0.1f;
	float angle = 0.01f;

public:

	void Initialize();

	void CameraMove(Input input);

	void Update(Input input);

	
	Matrix4x4 GetViewMatrix() { return viewMatrix_; }
	Transform GetCamera() { return camera_; }
};

