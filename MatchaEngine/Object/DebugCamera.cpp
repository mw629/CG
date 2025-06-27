#include "DebugCamera.h"

#include <cmath>

#include "Math/Calculation.h"

void DebugCamera::Initialize()
{
	pos_.offset = {0,0,0};
	pos_.radius = -30.0f;
	pos_.phi = 0.0f;//上下
	pos_.theta = 0.0f;//y軸

	camera_.translate = { 0,0,0 };
	camera_.scale = { 1.0,1.0,1.0 };
	camera_.rotate = { 0,0,0 };
}

void DebugCamera::Update(Input* input)
{
	if (input->PressKey(DIK_UP)) {
		pos_.phi += 0.1f / 60.0f;
	}
	
	if (input->PressKey(DIK_LEFT)) {
		pos_.theta += 0.1f / 60.0f;
	}

	camera_.translate = MakeWorldPos(pos_);
	cameraMatrix = MakeAffineMatrix(camera_.translate, camera_.scale, camera_.rotate);
	viewMatrix_ = Inverse(cameraMatrix);
}
