#include "Player.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include "../../Map/MapChipField.h"
#include "../../Map/MapStruct.h"
#include <numbers>

void Player::ImGui() {
#ifdef USE_IMGUI

	if (ImGui::CollapsingHeader("Player")) {
		ImGui::DragInt("HP", &HP_);
		if (ImGui::CollapsingHeader("Transform")) {
			ImGui::DragFloat3("Pos", &transform_.translate.x);
			ImGui::DragFloat3("Size", &transform_.scale.x);
			ImGui::DragFloat3("Rotate", &transform_.rotate.x);
		}
		if (ImGui::CollapsingHeader("Parameter")) {
			ImGui::DragFloat3("Velocity", &velocity_.x);//速度
			ImGui::DragFloat("Speed", &speed_, 0.0f, 2.0f);//速さ
			ImGui::DragFloat("WallFrictio", &wallFrictio, 0.0f, 1.0f);//壁との摩擦
		}
		if (ImGui::CollapsingHeader("Jump")) {
			ImGui::DragFloat("GravityAccleration", &kGravityAccleration);//重力
			ImGui::Checkbox("OnGround", &onGround_);//地面にいるフラグ
			ImGui::Checkbox("DoubleJump", &doubleJump);//ダブルジャンプフラグ
			ImGui::DragFloat("JumpAcceleration", &kJumpAcceleration);//ジャンプの初速度

		}
	}
#endif
}

void Player::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {
	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);

	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;
}

void Player::Update(Matrix4x4 viewMatrix) {

	MoveInput();

	invincibleFream--;

	MapCollision();

	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = { 0.0f,std::numbers::pi_v<float> };
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		float t = 1.0f - (turnTimer_ / kTimeTurn);
		transform_.rotate.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
	}

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Player::Draw() {
	Draw::DrawModel(model_.get());
}

void Player::MoveInput() {

	velocity_.x = 0;
	if (Input::PressKey(DIK_A) || Input::PressKey(DIK_D)) {
		if (Input::PressKey(DIK_A)) {
			velocity_.x += speed_ * -1.0f;
			lrDirection_ = LRDirection::kLeft;
		}
		if (Input::PressKey(DIK_D)) {
			velocity_.x += speed_;
			lrDirection_ = LRDirection::kRight;
		}
		// 回転開始処理
		turnTimer_ = kTimeTurn;
		turnFirstRotationY_ = transform_.rotate.y;
	}

	if (Input::PushKey(DIK_W) || GamePadInput::PushButton(XINPUT_GAMEPAD_A)) {
		if (onGround_) {

			velocity_.y = kJumpAcceleration; // 上向きの速度

		}
		else {
			 if (!doubleJump) {
				velocity_.y = kJumpAcceleration; // 上向きの速度
				doubleJump = true;
			}
		}
	}

	isShot_ = false;
	if (Input::PushKey(DIK_SPACE) || GamePadInput::PushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		isShot_ = true;
	}




	if (!onGround_) {
		// 落下速度
		velocity_ = velocity_ + Vector3(0.0f, -kGravityAccleration / 60.0f, 0.0f);
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
}





void Player::HitWall()
{
}

void Player::OnGround()
{
	doubleJump = false;
}

void Player::OnCollision(const Goal* goal)
{
	(goal);
}

void Player::OnCollision(const Enemy* enemy)
{
	if (invincibleFream < 0) {
		HP_--;
		invincibleFream = invincibleTime;
	}
	if (HP_ == 0) {
		isDead_ = true;
	}
}

