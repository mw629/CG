#include "NormalBullet.h"
#include "../Player.h"


void NormalBullet::Initialize(MapChipField* mapChipField)
{
	ModelData modelData = LoadObjFile("resources/Bullet", "Bullet.obj");
	model_ = std::make_unique<Model>();
	model_->Initialize(modelData);
	SetMapChipField(mapChipField);

	// キャラクターの当たり判定サイズ
	kWidth = 0.8f;
	kHeight = 0.8f;
}

void NormalBullet::Update(Matrix4x4 viewMatrix)
{
	fream_ -= 1.0f / 60.0f;

	if (fream_ < 0.0f) {
		isActive_ = false;
	}

	MapCollision();

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void NormalBullet::Draw()
{
	if (isActive_) {
		Draw::DrawModel(model_.get());
	}
}

void NormalBullet::IsShot(Player* player)
{
	transform_.translate = player->GetTransform().translate;
	fream_ = 1.0f;
	if (player->GetDirection() == LRDirection::kLeft) {
		velocity_.x = -speed_ / 10.0f;
	}
	else {
		velocity_.x = speed_ / 10.0f;
	}
	velocity_.y = 0.0f;  // Y方向の速度を初期化
	isActive_ = true;
}

void NormalBullet::HitWall()
{
	isActive_ = false;
}

void NormalBullet::OnCollision(const Enemy* enemy)
{
	(enemy);
	if (isActive_) {
		isActive_ = false;
	}
}

void NormalBullet::OnCollision(const BOSS* enemy)
{
	(enemy);
	if (isActive_) {
		isActive_ = false;
	}
}