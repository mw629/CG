#include "Enemy.h"

void Enemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	model_->GetMatrial()->SetColor(color);
	isActive_ = true;
}

void Enemy::Update(Matrix4x4 viewMatrix) {

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

AABB Enemy::GetAABB() {
	Vector3 worldPos = transform_.translate;
	AABB aabb;
	aabb.min = { worldPos.x - size_.x / 2.0f, worldPos.y - size_.y / 2.0f, worldPos.z - size_.x / 2.0f };
	aabb.max = { worldPos.x + size_.x / 2.0f, worldPos.y + size_.y / 2.0f, worldPos.z + size_.x / 2.0f };

	return aabb;
}
