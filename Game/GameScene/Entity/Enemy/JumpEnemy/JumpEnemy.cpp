#include "JumpEnemy.h"
#define NOMINMAX

#include <algorithm>


void JumpEnemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix)
{
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	model_->GetMatrial()->SetColor(color);
	isActive_ = true;

	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;
}

void JumpEnemy::Update(Matrix4x4 viewMatrix)
{
	Move();

	MapCollision();



	model_.get()->SetTransform(transform_);
	model_.get()->SettingWvp(viewMatrix);
}

void JumpEnemy::Move()
{

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

