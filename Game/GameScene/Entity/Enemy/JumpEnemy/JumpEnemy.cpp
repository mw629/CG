#include "JumpEnemy.h"
#define NOMINMAX

#include <algorithm>


void JumpEnemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix)
{
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	transform_.scale = {1.0f, 1.0f, 1.0f};
	initialScale_ = transform_.scale;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	color_ = { 1.0f,1.0f,0.0f,1.0f };
	model_->GetMatrial()->SetColor(color_);
	isActive_ = true;
	isDead_ = false;
	isPlayingDeathAnimation_ = false;
	deathAnimationTimer_ = 0.0f;

	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;

	deathSE_ = Audio::Load("resources/Audio/SE/gou.mp3");
}

void JumpEnemy::Update(Matrix4x4 viewMatrix)
{
	// 死亡演出中の場合
	if (isPlayingDeathAnimation_) {
		DeathAnimation();
		model_->SetTransform(transform_);
		model_->SettingWvp(viewMatrix);
		return;
	}

	Move();

	MapCollision();



	model_.get()->SetTransform(transform_);
	model_.get()->SettingWvp(viewMatrix);
}

void JumpEnemy::Move()
{
	velocity_.x = runSpeed_/60.0f;

	if (canJump) {
		JumpCoolTime = JumpCoolTimeMax;
		canJump = false;
		velocity_.y = kJumpAcceleration;
	}
	if (onGround_) {
		JumpCoolTime -= 1.0f / 60.0f;
		if (JumpCoolTime < 0) {
			canJump = true;
		}
	}

	if (!onGround_) {
		canJump = false;
		// 落下速度
		velocity_ = velocity_ + Vector3(0.0f, -gravity_ / 60.0f, 0.0f);
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
}

void JumpEnemy::HitWall()
{
	runSpeed_ *= -1.0f;
}


