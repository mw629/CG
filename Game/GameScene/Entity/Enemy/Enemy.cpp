#include "Enemy.h"

void Enemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	model_->GetMatrial()->SetColor(color_);
	isActive_ = true;

	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;
}

void Enemy::Update(Matrix4x4 viewMatrix) {

	moveTimer_ += moveSpeed_;

	// sin波で左右に揺らすっちゃ
	transform_.rotate.y = std::sin(moveTimer_) * moveRange_;

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Enemy::Draw() {
	if (isActive_) {
		Draw::DrawModel(model_.get());
	}
}

void Enemy::OnCollision(const Bullet* bullet) {
	(bullet);
	isActive_ = false;
}