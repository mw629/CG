#include "Camera.h"
#include <Engine.h>
#include "Graphics/GraphicsDevice.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI


Camera::Camera()
{
	Initialize();
}

void Camera::ImGui()
{
#ifdef _USE_IMGUI
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &transform_.translate.x);
		ImGui::DragFloat3("CameraSize", &transform_.scale.x);
		ImGui::DragFloat3("CameraRotate", &transform_.rotate.x);
	}
	ImGui::Checkbox("debugCamera", &isDebugCamera_);
#endif // _USE_IMGUI
}

void Camera::Initialize() {
	debugCamera_.Initialize();
	viewMatrix_ = Inverse(MakeAffineMatrix(transform_.translate,transform_.scale,transform_.rotate));
	
	// カメラ用のConstantBufferを作成
	cameraResource_ = GraphicsDevice::CreateBufferResource(sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
	
	// 初期値を設定
	cameraData_->worldPosition = transform_.translate;
}

void Camera::Update() {
    debugCamera_.Update();

    // カメラの位置・回転を決める
    Vector3 eye;
    Vector3 target;
    Vector3 up = { 0.0f, 1.0f, 0.0f };

    if (isDebugCamera_) {
        viewMatrix_ = debugCamera_.GetViewMatrix();
        eye = debugCamera_.GetEye();
    }
    else {
        Matrix4x4 cameraMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
        viewMatrix_ = Inverse(cameraMatrix);
        eye = transform_.translate;
    }

    cameraData_->worldPosition = eye;
}

