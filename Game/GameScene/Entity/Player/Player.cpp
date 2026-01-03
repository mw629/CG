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
		if (ImGui::CollapsingHeader("Death")) {
			ImGui::Checkbox("IsDead", &isDead_);
			ImGui::DragFloat("DeathTimer", &deathAnimationTimer_);
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

	// 死亡演出中は通常の更新処理をスキップ
	if (isDead_) {
		DeathAnimation();
		model_->SetTransform(transform_);
		model_->SettingWvp(viewMatrix);
		return;
	}

	MoveInput();

	invincibleFream--;

	MapCollision();

	JumpAnimation();

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

	// コヨーテタイムの更新
	if (onGround_) {
		coyoteTime_ = kCoyoteTimeDuration; // 地面にいる間はリセット
	} else {
		coyoteTime_ -= 1.0f / 60.0f; // 空中では減らす
	}

	if (Input::PushKey(DIK_W) || GamePadInput::PushButton(XINPUT_GAMEPAD_A)) {
		// 地面にいるか、コヨーテタイム内ならジャンプ可能
		if (onGround_ || coyoteTime_ > 0.0f) {
			velocity_.y = kJumpAcceleration; // 上向きの速度
			isJump_ = true;
			coyoteTime_ = 0.0f; // ジャンプしたらコヨーテタイムを消費
		}
		else {
			 if (!doubleJump) {
				velocity_.y = kJumpAcceleration; // 上向きの速度
				doubleJump = true;
				isJump_ = true;
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


void Player::JumpAnimation()
{
	if (!isJump_) {
		animationFream_ = 0;
		transform_.scale = { 1.0f, 1.0f, 1.0f };
		return;
	}
	else {
		animationFream_++;
	}

	// 縮む・伸びるアニメーション時間
	const int shrinkTime = 5;   // 縮むフレーム
	const int stretchTime = 10; // 伸びるフレーム
	const int totalTime = shrinkTime + stretchTime + 10; // 空中→戻り時間も含めて調整

	// 🔥変化幅を強める（前より派手）　
	const float minY = 0.5f;
	const float maxY = 1.1f;
	const float maxX = 1.1f;
	const float minX = 0.5f;


	if (animationFream_ < shrinkTime) {
		// 地面から飛び上がる前 → 縦に縮む
		float t = (float)animationFream_ / shrinkTime;
		transform_.scale.y = Lerp(1.0f, minY, t); // 縮む
		transform_.scale.x = Lerp(1.0f, maxX, t); // 横に広がる
	}
	else if (animationFream_ < shrinkTime + stretchTime) {
		// 上昇中 → びよーんと伸びる
		float t = (float)(animationFream_ - shrinkTime) / stretchTime;
		transform_.scale.y = Lerp(minY, maxY, t); // 伸びる
		transform_.scale.x = Lerp(maxX, minX, t); // 横が細くなる
	}
	else {
		// 空中で少しずつ元に戻る
		transform_.scale.y = Lerp(transform_.scale.y, 1.0f, 0.15f);
		transform_.scale.x = Lerp(transform_.scale.x, 1.0f, 0.15f);
	}

	// ⏱ 一定時間経過でジャンプ終了
	if (animationFream_ >= totalTime) {
		isJump_ = false;
		animationFream_ = 0;
		transform_.scale = { 1.0f, 1.0f, 1.0f };
	}
}

void Player::DeathAnimation()
{
	// タイマーを進める
	deathAnimationTimer_ += 1.0f / 60.0f;

	// 正規化された時間 (0.0 ~ 1.0)
	float t = deathAnimationTimer_ / kDeathAnimationDuration;
	t = (std::min)(t, 1.0f);

	// 演出パターン1: 縮みながら回転して消える
	transform_.scale = Vector3(
		Lerp(1.0f, 0.0f, t),
		Lerp(1.0f, 0.0f, t),
		Lerp(1.0f, 0.0f, t)
	);
	transform_.rotate.y += 0.1f; // 回転
	
	// 演出パターン2: 倒れる演出（X軸回転）
	//transform_.rotate.x = Lerp(0.0f, std::numbers::pi_v<float> / 2.0f, t);
	
	// 演出パターン3: 下に沈む演出
	// transform_.translate.y -= 0.02f;
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
		deathAnimationTimer_ = 0.0f; // タイマー初期化
	}
}

