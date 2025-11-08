#include "Goal.h"



void Goal::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {
	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	isActive_ = true;
}

void Goal::Update(Matrix4x4 viewMatrix) {

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Goal::Draw() {
	Draw::DrawModel(model_.get());
}

AABB Goal::GetAABB() {
	Vector3 worldPos = transform_.translate;
	AABB aabb;
	aabb.min = { worldPos.x - size_.x / 2.0f, worldPos.y - size_.y / 2.0f, worldPos.z - size_.x / 2.0f };
	aabb.max = { worldPos.x + size_.x / 2.0f, worldPos.y + size_.y / 2.0f, worldPos.z + size_.x / 2.0f };

	return aabb;
}

