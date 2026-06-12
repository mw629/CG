#include "Obstacle.h"

Obstacle::Obstacle()
{
}

Obstacle::~Obstacle()
{
}

void Obstacle::Initialize(ModelData modelData, Type type)
{
	model_->Initialize(modelData);
	SetType(type);
}

void Obstacle::SetType(Type type)
{
	type_ = type;
	model_->SetShader("ObjectShader");

	// タイプに応じて当たり判定サイズとスケールを設定
	switch (type_)
	{
	case Type::Low:
		// ジャンプで避ける低い障害物
		collisionWidth_ = 1.5f;
		collisionHeight_ = 1.0f;
		collisionDepth_ = 1.0f;
		transform_.scale = { 1.5f, 1.0f, 1.0f };
		model_->GetMartial()->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f }); // 緑色
		break;
	case Type::High:
		// 転がりで避ける高い障害物（上に浮いている）
		collisionWidth_ = 1.5f;
		collisionHeight_ = 3.0f;
		collisionDepth_ = 1.0f;
		transform_.scale = { 1.5f, 3.0f, 1.0f };
		model_->GetMartial()->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		break;
	case Type::Wall:
		// レーン移動で避ける壁
		collisionWidth_ = 1.5f;
		collisionHeight_ = 3.0f;
		collisionDepth_ = 1.0f;
		transform_.scale = { 1.5f, 3.0f, 1.0f };
		model_->GetMartial()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 白色
		model_->SetShader("IceShader");
		break;
	}

	model_->SetTransform(transform_);
}

void Obstacle::Spawn(float x, float y, float z)
{
	transform_.translate = { x, y, z };
	isActive_ = true;
	model_->SetTransform(transform_);
}

void Obstacle::Update(Matrix4x4 view, float scrollSpeed)
{
	if (!isActive_) return;

	// 手前に向かってスクロール
	transform_.translate.z -= scrollSpeed;

	// 画面の後ろ（手前）を過ぎたら非アクティブ
	if (transform_.translate.z < -10.0f) {
		isActive_ = false;
	}

	model_->SetTransform(transform_);
	model_->SettingWvp(view);
}

void Obstacle::Draw()
{
	if (!isActive_) return;
	Draw::DrawObj(model_.get());
}
