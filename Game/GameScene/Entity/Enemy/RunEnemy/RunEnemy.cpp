#include "RunEnemy.h"
#define NOMINMAX

#include <algorithm>


void RunEnemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix)
{
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	transform_.scale = {1.0f, 1.0f, 1.0f};
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
	float kWidth = 0.9f;
	float kHeight = 0.9f;
}

void RunEnemy::Update(Matrix4x4 viewMatrix)
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

void RunEnemy::Move()
{
	velocity_.x = runSpeed_ / 60.0f;

	if (!onGround_) {
		// 落下速度
		velocity_ = velocity_ + Vector3(0.0f, -gravity_ / 60.0f, 0.0f);
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
}

void RunEnemy::HitWall()
{
	runSpeed_ *= -1.0f;
}


