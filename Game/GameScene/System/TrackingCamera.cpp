#define NOMINMAX

#include "TrackingCamera.h"
#include "../Entity/Player/Player.h"
#include <algorithm>
#include <imgui.h>

void TrackingCamera::ImGui()
{
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &transform_.translate.x);
		ImGui::DragFloat3("CameraSize", &transform_.scale.x);
		ImGui::DragFloat3("CameraRotate", &transform_.rotate.x);
	}
}

void TrackingCamera::Initialize(Player* player)
{
	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
	
	target_ = player;
	
	// カメラの初期位置を設定
	if (target_) {
		Transform targetTransform = target_->GetTransform();
		transform_.translate = AddVector3(targetTransform.translate, targeOffset);
		transform_.scale = { 1.0f, 1.0f, 1.0f };
		transform_.rotate = { 0.0f, 0.0f, 0.0f };
		
		camera_.get()->SetTransform(transform_);
		camera_.get()->Update();
	}
}

void TrackingCamera::Update() {

	Transform targetTransform = target_->GetTransform();
	
	// プレイヤーの速度を取得
	targetVelocity_ = target_->GetVelocity();

	targetLocation_ = AddVector3(AddVector3(targetTransform.translate, targeOffset), ScalarMultiply(targetVelocity_, kVelocityBias));

	transform_.translate = Lerp(transform_.translate, targetLocation_, kInterpolationRate);

	transform_.translate.x = std::clamp(transform_.translate.x, targetTransform.translate.x + Margin.left, targetTransform.translate.x + Margin.right);
	transform_.translate.y = std::clamp(transform_.translate.y, targetTransform.translate.y + Margin.bottom, targetTransform.translate.y + Margin.top);

	camera_.get()->SetTransform(transform_);
	camera_.get()->Update();
}

void TrackingCamera::Reset() {
	if (!target_) return;

	Transform targetTransform = target_->GetTransform();
	transform_.translate = AddVector3(targetTransform.translate, targeOffset);
	camera_.get()->SetTransform(transform_);
	camera_.get()->Update();
}
