#include "Camera.h"
#include <Engine.h>
#include <imgui.h>

void Camera::ImGui()
{
	ImGui::Checkbox("debugCamera", &isDebugCamera_);
}

void Camera::Initialize() {
	debugCamera_.Initialize();
	viewMatrix_ = Inverse(MakeAffineMatrix(transform_.translate,transform_.scale,transform_.rotate));
}

void Camera::Update() {
	debugCamera_.Update();
	viewMatrix_ = Inverse(MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate));

	if (isDebugCamera_) {
		viewMatrix_ = debugCamera_.GetViewMatrix();
	}
}


