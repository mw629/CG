#include "JumpEnemy.h"


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
}
