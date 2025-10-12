#include "Camera.h"
#include <Engine.h>
#include <imgui.h>

void Camera::ImGui()
{
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &transform_.translate.x, 0.1f);
		ImGui::DragFloat3("CameraSize", &transform_.scale.x, 0.1f);
		ImGui::DragFloat3("CameraRoteta", &transform_.rotate.x, 0.1f);
	}
}

void Camera::Initialize() {
	viewMatrix_ = Inverse(MakeAffineMatrix(transform_.translate,transform_.scale,transform_.rotate));
}

void Camera::Update() {
	viewMatrix_ = Inverse(MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate));
}


