#include "StopEnemy.h"
#define NOMINMAX

#include <algorithm>


void StopEnemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix)
{
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	transform_.scale = {1.0f, 1.0f, 1.0f};
	initialScale_ = transform_.scale;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	color_ = { 0.0f,0.0f,0.0f,1.0f };
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

void StopEnemy::Update(Matrix4x4 viewMatrix)
{
	// 死亡演出中の場合
	if (isPlayingDeathAnimation_) {
		DeathAnimation();
		model_->SetTransform(transform_);
		model_->SettingWvp(viewMatrix);
		return;
	}

	MapCollision();

	model_.get()->SetTransform(transform_);
	model_.get()->SettingWvp(viewMatrix);
}


