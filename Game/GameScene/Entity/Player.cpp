#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize(ModelData modelData)
{
	transform_.translate.y = baseHeight_;
	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(transform_);
}

void Player::Reset()
{
	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.translate = { 0.0f, baseHeight_, 0.0f };

	laneIndex_ = 0;
	targetLaneIndex_ = 0;
	lerpTime_ = 0.0f;
	startX_ = 0.0f;
	moveDirection_ = MoveDirection::None;

	isJumping_ = false;
	velocityY_ = 0.0f;
	isRolling_ = false;
	rollTimer_ = 0.0f;
	keepRolling_ = false;

	isHit_ = false;
	isTrip_ = false;
	hitTimer_ = 0.0f;
	knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };

	model_.get()->SetTransform(transform_);
}

void Player::Update(Matrix4x4 view, float speedMultiplier)
{
	if (isHit_) {
		// SpeedMultiplier is ignored for hit update so animation plays consistently 
		// even if the game scroll stops.
		HitUpdate(1.0f);
	} else {
		PlayerMove(speedMultiplier);
	}

	model_.get()->SettingWvp(view);
}

void Player::PlayerMove(float speedMultiplier)
{
	const float kLaneWidth = 2.0f; // レーンの横幅
	const int kMinLane = -1;       // 一番左のレーン
	const int kMaxLane = 1;        // 一番右のレーン

	if (speedMultiplier <= 0.0f) {
		model_.get()->SetTransform(transform_);
		return;
	}

	// レーンの移動中ではなかったら
	if (laneIndex_ == targetLaneIndex_) {
		// キー入力で目標レーンを設定
		if (Input::PushKey(DIK_A)||Input::PushKey(DIK_LEFT)) {
			targetLaneIndex_ = laneIndex_ - 1;
		}
		if (Input::PushKey(DIK_D)||Input::PushKey(DIK_RIGHT)) {
			targetLaneIndex_ = laneIndex_ + 1;
		}

		// レーンの範囲制限
		if (targetLaneIndex_ < kMinLane) targetLaneIndex_ = kMinLane;
		if (targetLaneIndex_ > kMaxLane) targetLaneIndex_ = kMaxLane;

		// 移動が開始される場合、初期値を保存
		if (targetLaneIndex_ != laneIndex_) {
			startX_ = transform_.translate.x;
			lerpTime_ = 0.0f;

			// 横移動時にしゃがみ（転がり）をキャンセルして硬直をなくす
			if (isRolling_ && !keepRolling_) {
				isRolling_ = false;
				transform_.scale.y = 1.0f;
				transform_.translate.y = baseHeight_;
			}
		}
	}
	// レーンの移動中だったら
	else {
		// 線形補間で移動
		lerpTime_ += laneChangeSpeed_ * speedMultiplier;
		if (lerpTime_ > 1.0f) {
			lerpTime_ = 1.0f;
		}

		float targetX = static_cast<float>(targetLaneIndex_) * kLaneWidth;
		transform_.translate.x = Lerp(startX_, targetX, lerpTime_);

		// 移動が完了したら現在のレーンを更新
		if (lerpTime_ >= 1.0f) {
			laneIndex_ = targetLaneIndex_;
		}
	}

	// === アクション（ジャンプと転がり） ===
	// 地上にいてジャンプ中でなければアクション可能（転がり中でもジャンプでキャンセル可能）
	if (!isJumping_) {
		if (Input::PushKey(DIK_W) || Input::PushKey(DIK_SPACE)||Input::PushKey(DIK_UP)) {
			if (!(isRolling_ && keepRolling_)) {
				isJumping_ = true;
				velocityY_ = jumpPower_ * speedMultiplier;

				// ジャンプ時にしゃがみをキャンセル
				if (isRolling_) {
					isRolling_ = false;
					transform_.scale.y = 1.0f;
					transform_.translate.y = baseHeight_;
				}
			}
		}
		else if (!isRolling_ && (Input::PushKey(DIK_S) || Input::PushKey(DIK_DOWN))) {
			isRolling_ = true;
			rollTimer_ = rollDuration_;
			// 転がり中はスケールYを半分にして伏せるようにする
			transform_.scale.y = 0.5f;
			// 重心が変わる分、Y座標を少し下げる（原点が中心の場合）
			transform_.translate.y = baseHeight_ - 0.5f; 
		}
	}

	// ジャンプ処理
	if (isJumping_) {
		transform_.translate.y += velocityY_;
		velocityY_ -= gravity_ * (speedMultiplier * speedMultiplier);

		// 地面に着地
		if (transform_.translate.y <= baseHeight_) {
			transform_.translate.y = baseHeight_;
			isJumping_ = false;
			velocityY_ = 0.0f;
		}
	}

	// 転がり処理
	if (isRolling_) {
		rollTimer_ -= speedMultiplier;
		if (rollTimer_ <= 0.0f && !keepRolling_) {
			isRolling_ = false;
			// 姿勢を元に戻す
			transform_.scale.y = 1.0f;
			transform_.translate.y = baseHeight_;
		}
	}

	// トランスフォームをモデルに適用
	model_.get()->SetTransform(transform_);
}

void Player::Draw()
{
	Draw::DrawObj(model_.get());
}

void Player::ImGui()
{
	GameObject::ImGui();
	if (model_) {
		model_->ImGui();
	}
}

void Player::HitUpdate(float /*speedMultiplier*/)
{
	// ノックバック処理
	if (isHit_) {
		hitTimer_ += 1.0f;

		transform_.translate.x += knockbackVelocity_.x;
		transform_.translate.y += knockbackVelocity_.y;
		transform_.translate.z += knockbackVelocity_.z;

		// 重力と回転（後ろに飛ぶか前に転がるか）
		knockbackVelocity_.y -= gravity_ * 2.0f; 
		if (isTrip_) {
			transform_.rotate.x += 0.2f; // 前に転がる回転
		} else {
			transform_.rotate.x -= 0.1f; // 後ろに飛ぶ回転
		}

		// 地面に着地したらバウンドなどを抑える
		if (transform_.translate.y <= baseHeight_ && knockbackVelocity_.y < 0.0f) {
			transform_.translate.y = baseHeight_;
			knockbackVelocity_.y = 0.0f;
			knockbackVelocity_.x *= 0.8f;
			knockbackVelocity_.z *= 0.8f;
		}
		
		model_.get()->SetTransform(transform_);
	}
}

void Player::OnHit(bool isTrip)
{
	isHit_ = true;
	isTrip_ = isTrip;
	hitTimer_ = 0.0f;
	
	// 姿勢をリセット
	isRolling_ = false;
	isJumping_ = false;
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	float randX = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;

	if (isTrip_) {
		// Lowに当たってつまずいた場合、少し前に転がるようなノックバック
		knockbackVelocity_ = { randX, 0.3f, 0.8f }; 
	} else {
		// 少し後ろと上に飛ぶノックバック
		knockbackVelocity_ = { randX, 0.4f, -0.6f }; 
	}
}

bool Player::IsHitAnimationFinished() const
{
	return isHit_ && hitTimer_ >= hitDuration_;
}
