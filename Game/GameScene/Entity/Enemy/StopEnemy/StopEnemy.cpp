#include "StopEnemy.h"
#define NOMINMAX

#include <algorithm>


void StopEnemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix)
{
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	color_ = { 0.0f,0.0f,0.0f,1.0f };
	model_->GetMatrial()->SetColor(color_);
	isActive_ = true;

	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;
}

void StopEnemy::Update(Matrix4x4 viewMatrix)
{

	MapCollision();


	model_.get()->SetTransform(transform_);
	model_.get()->SettingWvp(viewMatrix);
}


