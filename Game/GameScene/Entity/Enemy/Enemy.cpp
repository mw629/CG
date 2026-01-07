#define NOMINMAX
#include "Enemy.h"
#include <algorithm>
#include <cmath>


void Enemy::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {
	ModelData modelData = LoadObjFile("resources/Enemy", "Enemy.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	transform_.scale = {1.0f, 1.0f, 1.0f};
	initialScale_ = transform_.scale;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	model_->GetMatrial()->SetColor(color_);
	isActive_ = true;
	isDead_ = false;
	isPlayingDeathAnimation_ = false;
	deathAnimationTimer_ = 0.0f;

	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;

	// 死亡効果音のロード
	deathSE_ = Audio::Load("resources/Audio/SE/gou.mp3");
}

void Enemy::Update(Matrix4x4 viewMatrix) {
	// 死亡演出中の場合
	if (isPlayingDeathAnimation_) {
		DeathAnimation();
		model_->SetTransform(transform_);
		model_->SettingWvp(viewMatrix);
		return;
	}

	moveTimer_ += moveSpeed_;

	// sin波で左右に揺らすっちゃ
	transform_.rotate.y = std::sin(moveTimer_) * moveRange_;

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Enemy::DeathAnimation() {
	deathAnimationTimer_ += 0.016f; // 約60FPS想定

	// 正規化された進行度 (0.0 ~ 1.0)
	float progress = std::min(deathAnimationTimer_ / kDeathAnimationDuration_, 1.0f);

	// フェードアウト (透明度を減少)
	color_.w = 1.0f - progress;
	model_->GetMatrial()->SetColor(color_);

	// 回転しながら消える
	transform_.rotate.y += 0.1f;
	transform_.rotate.x += 0.05f;

	// 縮小しながら消える
	float scale = 1.0f - progress;
	transform_.scale = {
		initialScale_.x * scale,
		initialScale_.y * scale,
		initialScale_.z * scale
	};

	// 少し上に浮き上がる演出
	transform_.translate.y += 0.02f * (1.0f - progress);

	// 演出終了時に非アクティブ化
	if (progress >= 1.0f) {
		isActive_ = false;
		isPlayingDeathAnimation_ = false;
	}
}

void Enemy::Draw() {
	if (isActive_ || isPlayingDeathAnimation_) {
		Draw::DrawModel(model_.get());
	}
}

void Enemy::OnCollision(const Bullet* bullet) {
	(bullet);
	// 既に死亡している、または演出中の場合は何もしない
	if (isDead_ || isPlayingDeathAnimation_) {
		return;
	}
	
	isDead_ = true;
	isPlayingDeathAnimation_ = true;
	deathAnimationTimer_ = 0.0f;

	// 死亡効果音を再生
	if (deathSE_ >= 0) {
		Audio::Play(deathSE_, false,10.0f);
	}
}