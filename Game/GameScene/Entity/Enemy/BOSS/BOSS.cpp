#include "BOSS.h"
#include "../../Player/Player.h"
#include <cmath>
#include <Math/Calculation.h>

// フレーム時間はヘッダのメンバを利用するためここでは定数にしない

float BOSS::EaseInOutCubic(float t) const {
	if (t < 0.5f) {
		return 4.0f * t * t * t;
	}
	float f = (2.0f * t) - 2.0f;
	return 0.5f * (f * f * f + 2.0f);
}

void BOSS::StartMoveToPlayer(float playerX)
{
	moveTimer_ = 0.0f;
	moveStartX_ = transform_.translate.x;
	moveTargetX_ = playerX;
	bossState_ = BossState::kMoveToPlayerX;
	actionStarted_ = true;
}

void BOSS::UpdateMoveToPlayer()
{
	moveTimer_ += kDeltaTime;
	float t = moveTimer_ / moveDuration_;
	if (t > 1.0f) t = 1.0f;
	float e = EaseInOutCubic(t);
	transform_.translate.x = Lerp(moveStartX_, moveTargetX_, e);

	if (t >= 1.0f) {
		// 移動完了→落下状態へ
		StartFall();
	}
}

void BOSS::StartFall()
{
	bossState_ = BossState::kFall;
	velocity_.y = 0.0f;
	onGround_ = false; 
}

void BOSS::UpdateFall()
{
	// 重力を適用（下向きは負方向）
	velocity_.y -= gravity_ * kDeltaTime;
	if (velocity_.y < maxFallSpeed_) velocity_.y = maxFallSpeed_;

	MapCollision();

	if (onGround_) {
		fallFream_++;
	}
	if (fallFream_ >= 30.0f) {
		fallFream_ = 0.0f;
		StartReturnToReference();
	}
}

void BOSS::StartReturnToReference()
{
	bossState_ = BossState::kReturnToReference;
	// 現在位置を開始位置にする（着地位置）
	returnStartPos_ = transform_.translate;
	// 最終ターゲットは基準点
	returnTargetPos_ = ReferencePoint_;
	// フェーズを縦上昇に設定
	returnPhase_ = ReturnPhase::kAscendToReferenceY;
	returnTimer_ = 0.0f;
}

void BOSS::UpdateReturnToReference()
{
	if (returnPhase_ == ReturnPhase::kAscendToReferenceY) {
		// 縦に ReferencePoint_.y まで上がる（X は着地時の X を維持）
		returnTimer_ += kDeltaTime;
		float t = returnTimer_ / returnVerticalDuration_;
		if (t > 1.0f) t = 1.0f;
		float e = EaseInOutCubic(t);

		// X は着地時のまま
		transform_.translate.x = returnStartPos_.x;
		// Y は着地位置から ReferencePoint_.y へ補間
		transform_.translate.y = Lerp(returnStartPos_.y, ReferencePoint_.y, e);

		if (t >= 1.0f) {
			// 縦上昇完了 → 横移動フェーズへ
			returnPhase_ = ReturnPhase::kMoveHorizontallyToReferenceX;
			// 横移動の開始位置は現在位置（= ReferencePoint_.y を持つが x は着地x）
			returnStartPos_ = transform_.translate;
			returnTimer_ = 0.0f;
		}
	}
	else { // 横移動フェーズ
		returnTimer_ += kDeltaTime;
		float t = returnTimer_ / returnHorizontalDuration_;
		if (t > 1.0f) t = 1.0f;
		float e = EaseInOutCubic(t);

		transform_.translate.x = Lerp(returnStartPos_.x, ReferencePoint_.x, e);
		transform_.translate.y = ReferencePoint_.y;

		if (t >= 1.0f) {
			// 帰還完了
			bossState_ = BossState::kIdle;
			velocity_.y = 0.0f;
			onGround_ = true;
			actionStarted_ = false;
		}
	}
}

void BOSS::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {

	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	ReferencePoint_ = position;
	transform_.scale = { 2.0f, 2.0f, 2.0f };
	transform_.rotate = { 0.0f, -3.14f / 2.0f, 0.0f };
	initialScale_ = transform_.scale;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	color_ = { 0.0f,0.0f,1.0f,1.0f };
	model_->GetMatrial()->SetColor(color_);
	isActive_ = true;
	isDead_ = false;
	isPlayingDeathAnimation_ = false;
	deathAnimationTimer_ = 0.0f;

	// キャラクターの当たり判定サイズ
	float kWidth = 3.9f;
	float kHeight = 3.9f;

	deathSE_ = Audio::Load("resources/Audio/SE/gou.mp3");

	// 初期状態
	bossState_ = BossState::kIdle;
	moveTimer_ = 0.0f;
	returnTimer_ = 0.0f;
	actionStarted_ = false;

	// velocity_ は Entity にあるので初期化
	velocity_ = { 0.0f, 0.0f, 0.0f };
}

void BOSS::Update(Player* player, Matrix4x4 viewMatrix) {

	if (isPlayingDeathAnimation_) {
		DeathAnimation();
		model_->SetTransform(transform_);
		model_->SettingWvp(viewMatrix);
		return;
	}

	// 自動でシーケンスを開始（未開始時かつプレイヤー指定で即開始）
	if (!actionStarted_ && player) {
		StartMoveToPlayer(player->GetTransform().translate.x);
	}

	// 状態更新
	switch (bossState_) {
	case BossState::kIdle:
		// 何もしない
		break;

	case BossState::kMoveToPlayerX:
		UpdateMoveToPlayer();
		break;

	case BossState::kFall:
		UpdateFall();
		break;

	case BossState::kReturnToReference:
		UpdateReturnToReference();
		break;
	}

	model_.get()->SetTransform(transform_);
	model_.get()->SettingWvp(viewMatrix);
}

void BOSS::ResetAction()
{
	// 基準位置に戻してステートを初期化。これで Update が再び開始可能になる
	transform_.translate = ReferencePoint_;
	velocity_ = { 0.0f, 0.0f, 0.0f };
	moveTimer_ = 0.0f;
	returnTimer_ = 0.0f;
	bossState_ = BossState::kIdle;
	actionStarted_ = false;
	onGround_ = true;
	// フェーズリセット
	returnPhase_ = ReturnPhase::kAscendToReferenceY;
}

void BOSS::StartActionAtPlayerX(float playerX)
{
	// 外部から即時開始したい場合に使用
	ResetAction();
	StartMoveToPlayer(playerX);
}

void BOSS::Attack()
{

}

void BOSS::OnCollision(const Bullet* bullet) {
	(bullet);

	HP_--;

	if (HP_ <= 0) {
		isPlayingDeathAnimation_ = true;
	}
}
