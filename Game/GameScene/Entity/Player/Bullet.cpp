#include "Bullet.h"
#include "Player.h"


void Bullet::ImGui() {

}

void Bullet::Initialize() {

	ModelData modelData = LoadObjFile("resources/Bullet", "Bullet.obj");
	model_ = std::make_unique<Model>();
	model_->Initialize(modelData);
}

void Bullet::Update(Matrix4x4 viewMatrix) {

	fream_ -= 1.0f / 60.0f;

	if (fream_ < 0.0f) {
		isActiv_ = false;
	}
	transform_.translate += velocity_;

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Bullet::Draw() {
	if (isActiv_) {
		Draw::DrawModel(model_.get());
	}
}

void Bullet::IsShot(Player* player)
{
	transform_.translate = player->GetTransform().translate;
	fream_ = 1.0f;
	if (player->GetDirection() == LRDirection::kLeft) {
		velocity_.x = -speed_ / 10.0f;
	}
	else {
		velocity_.x = speed_ / 10.0f;
	}
	isActiv_ = true;
}

void Bullet::OnCollision(const Enemy* enemy)
{
	(enemy);
	if (isActiv_) {
		isActiv_ = false;
	}
}



AABB Bullet::GetAABB()
{
	Vector3 worldPos = transform_.translate;
	AABB aabb;
	aabb.min = { worldPos.x - size_.x / 2.0f, worldPos.y - size_.y / 2.0f, worldPos.z - size_.x / 2.0f };
	aabb.max = { worldPos.x + size_.x / 2.0f, worldPos.y + size_.y / 2.0f, worldPos.z + size_.x / 2.0f };

	return aabb;
}