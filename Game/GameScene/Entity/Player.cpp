#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize(ModelData modelData)
{
	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(transform_);
}

void Player::Update(Matrix4x4 view)
{
	PlayerMove();


	model_.get()->SettingWvp(view);
}

void Player::PlayerMove()
{
	const float kLaneWidth = 2.0f; // レーンの横幅
	const int kMinLane = -1;       // 一番左のレーン
	const int kMaxLane = 1;        // 一番右のレーン

	// レーンの移動中ではなかったら
	if (laneIndex_ == targetLaneIndex_) {
		// キー入力で目標レーンを設定
		if (Input::PushKey(DIK_A)) {
			targetLaneIndex_ = laneIndex_ - 1;
		}
		if (Input::PushKey(DIK_D)) {
			targetLaneIndex_ = laneIndex_ + 1;
		}

		// レーンの範囲制限
		if (targetLaneIndex_ < kMinLane) targetLaneIndex_ = kMinLane;
		if (targetLaneIndex_ > kMaxLane) targetLaneIndex_ = kMaxLane;

		// 移動が開始される場合、初期値を保存
		if (targetLaneIndex_ != laneIndex_) {
			startX_ = transform_.translate.x;
			lerpTime_ = 0.0f;
		}
	}
	// レーンの移動中だったら
	else {
		// 線形補間で移動
		lerpTime_ += laneChangeSpeed_;
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
	// 地上にいて何もしていない時のみアクション可能
	if (!isJumping_ && !isRolling_) {
		if (Input::PushKey(DIK_W) || Input::PushKey(DIK_SPACE)) {
			isJumping_ = true;
			velocityY_ = jumpPower_;
		}
		else if (Input::PushKey(DIK_S)) {
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
		velocityY_ -= gravity_;

		// 地面に着地
		if (transform_.translate.y <= baseHeight_) {
			transform_.translate.y = baseHeight_;
			isJumping_ = false;
			velocityY_ = 0.0f;
		}
	}

	// 転がり処理
	if (isRolling_) {
		rollTimer_ -= 1.0f;
		if (rollTimer_ <= 0.0f) {
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
