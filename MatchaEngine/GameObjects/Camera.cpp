#include "Camera.h"
#include <Engine.h>

#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI

void Camera::ImGui()
{
#ifdef _USE_IMGUI

	ImGui::Checkbox("debugCamera", &isDebugCamera_);
#endif // _USE_IMGUI
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


