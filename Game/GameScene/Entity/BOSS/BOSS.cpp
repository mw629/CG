#include "BOSS.h"
#include "../Player/Player.h"



void BOSS::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {

	//ModelData modelData = LoadObjFile("resources/BOSS", "BOSS.obj");
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	transform_.scale = { 1.0f, 1.0f, 1.0f };
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	color_ = { 1.0f,1.0f,0.0f,1.0f };
	model_->GetMatrial()->SetColor(color_);
	isActive_ = true;

	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;

	deathSE_ = Audio::Load("resources/Audio/SE/gou.mp3");

}

void BOSS::Update(Player* player, Matrix4x4 viewMatrix) {


	model_.get()->SetTransform(transform_);
	model_.get()->SettingWvp(viewMatrix);
}


void BOSS::Draw() {

	Draw::DrawModel(model_.get());

}

void BOSS::OnCollision(const Bullet* bullet) {
	(bullet);
	HP_--;
}
