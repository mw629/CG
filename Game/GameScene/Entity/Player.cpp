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

	// トランスフォームをモデルに適用
	model_.get()->SetTransform(transform_);
}

void Player::Draw()
{
	Draw::DrawObj(model_.get());
}
