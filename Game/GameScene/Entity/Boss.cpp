#include "Boss.h"
#include "Graphics/Render/Draw.h"
#include <cmath>

Boss::Boss() {
  name_ = "Boss";
  isActive_ = false;
}

Boss::~Boss() {}

void Boss::Initialize(ModelData modelData) {
  model_->Initialize(modelData);
  model_->SetShader("ObjectShader");
  model_->GetMartial()->SetColor({1.0f, 0.0f, 0.0f, 1.0f}); // ボスは赤色
}

void Boss::Spawn(float x, float y, float z) {
  transform_.translate = {x, y, z};
  transform_.scale = {5.0f, 5.0f, 5.0f}; // 大きめに設定
  isActive_ = true;
  hp_ = 20;
  isHit_ = false;
  hitTimer_ = 0.0f;
  model_->SetTransform(transform_);
}

void Boss::Update(Matrix4x4 view, float speedMultiplier) {
  if (!isActive_) return;

  // ふわふわ浮かぶアニメーション
  static float time = 0.0f;
  time += 0.05f * speedMultiplier;
  transform_.translate.y = 8.0f + std::sin(time) * 1.0f;

  if (isHit_) {
    hitTimer_ -= 0.016f; // 約1フレーム分
    if (hitTimer_ <= 0.0f) {
      isHit_ = false;
      model_->GetMartial()->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
    }
  }

  model_->SetTransform(transform_);
  model_->SettingWvp(view);
  GameObject::Update(view, speedMultiplier);
}

void Boss::Draw(class Draw &draw) {
  if (!isActive_) return;
  draw.DrawObj(model_.get());
  GameObject::Draw(draw);
}

void Boss::ImGuiInnerComponents() {
  if (model_) {
    model_->ImGui(false);
  }
}

void Boss::OnDamage() {
  hp_--;
  isHit_ = true;
  hitTimer_ = 0.5f; // 0.5秒間ダメージ演出
  model_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 白く光る
}
