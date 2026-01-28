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
		// expose per-axis interpolation rates for tuning
		ImGui::DragFloat("InterpRate X", &kInterpolationRateX, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat("InterpRate Y", &kInterpolationRateY, 0.0001f, 0.0f, 1.0f);
		ImGui::DragFloat("InterpRate Z", &kInterpolationRateZ, 0.001f, 0.0f, 1.0f);
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

	// 死亡時はプレイヤーに少し近づく
	if (target_->IsDead()) {
		// Z方向を近づける（プレイヤー位置に向かってオフセットを縮める）
		targetLocation_.z = targetTransform.translate.z + targeOffset.z * kDeathZoomFactor;
		// X, Y もプレイヤー中心に寄せる
		targetLocation_.x = targetTransform.translate.x;
		targetLocation_.y = targetTransform.translate.y + targeOffset.y;
	}

	// 軸ごとに異なる線形補間でスムーズに追従（Xを強め、Yを弱めるため）
	Vector3 newTranslate;
	newTranslate.x = Lerp(transform_.translate.x, targetLocation_.x, kInterpolationRateX);
	newTranslate.y = Lerp(transform_.translate.y, targetLocation_.y, kInterpolationRateY);
	newTranslate.z = Lerp(transform_.translate.z, targetLocation_.z, kInterpolationRateZ);
	transform_.translate = newTranslate;

	// 死亡時はマージン制限を無効化
	if (!target_->IsDead()) {
		// マージン内に制限
		transform_.translate.x = std::clamp(transform_.translate.x, targetTransform.translate.x + Margin.left, targetTransform.translate.x + Margin.right);
		transform_.translate.y = std::clamp(transform_.translate.y, targetTransform.translate.y + Margin.bottom, targetTransform.translate.y + Margin.top);
	}

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
