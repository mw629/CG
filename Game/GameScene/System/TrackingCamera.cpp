#define NOMINMAX

#include "TrackingCamera.h"
#include "../Entity/Player/Player.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui.h>
#endif

void TrackingCamera::ImGui()
{
#ifdef USE_IMGUI

	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &transform_.translate.x);
		ImGui::DragFloat3("CameraSize", &transform_.scale.x);
		ImGui::DragFloat3("CameraRotate", &transform_.rotate.x);
	}
#endif
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

	// 目標位置を計算（プレイヤー位置 + オフセット + 速度予測）
	targetLocation_ = AddVector3(AddVector3(targetTransform.translate, targeOffset), ScalarMultiply(targetVelocity_, kVelocityBias));

	// 線形補間でスムーズに追従
	transform_.translate = Lerp(transform_.translate, targetLocation_, kInterpolationRate);

	// マージン内に制限
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
