#include "Boss.h"
#include "Graphics/Render/Draw.h"
#include <cmath>
#include <imgui.h>

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
  targetPos_ = {x, y, z};
  // 画面の右奥上空（カメラはZ=-向きなので、X負が右、Z負が奥）
  startPos_ = {-18.0f, 20.0f, -40.0f}; 
  transform_.translate = startPos_;
  transform_.scale = {5.0f, 5.0f, 5.0f}; // 大きめに設定
  isActive_ = true;
  hp_ = 20;
  isHit_ = false;
  hitTimer_ = 0.0f;
  model_->SetTransform(transform_);
  ChangeState(BossState::Appearance);
}

void Boss::ChangeState(BossState nextState) {
  state_ = nextState;
  stateTimer_ = 0.0f;
}

void Boss::Update(Matrix4x4 view, float speedMultiplier) {
  if (!isActive_) return;

  stateTimer_ += 0.016f * speedMultiplier; // おおよその時間を加算

  switch (state_) {
  case BossState::Appearance: {
    // 出現時の演出（左奥から所定の位置へ飛んでくる）
    float t = stateTimer_ / 2.0f; // 2秒かけて登場
    if (t > 1.0f) t = 1.0f;
    
    // EaseOutQuad (だんだんゆっくりに)
    float easeT = 1.0f - (1.0f - t) * (1.0f - t);
    
    transform_.translate.x = startPos_.x + (targetPos_.x - startPos_.x) * easeT;
    transform_.translate.y = startPos_.y + (targetPos_.y - startPos_.y) * easeT;
    transform_.translate.z = startPos_.z + (targetPos_.z - startPos_.z) * easeT;
    
    // 登場時は回転しながら降りてくる
    transform_.rotate.y = (1.0f - easeT) * 3.14159f * 2.0f;

    // 出現演出でシェイクさせる (だんだん揺れが収まる)
    float shakeIntensity = (1.0f - easeT) * 2.0f;
    transform_.translate.x += std::sin(stateTimer_ * 50.0f) * shakeIntensity;
    transform_.translate.y += std::cos(stateTimer_ * 65.0f) * shakeIntensity;
    transform_.translate.z += std::sin(stateTimer_ * 40.0f) * shakeIntensity;

    if (stateTimer_ > 2.0f) {
      transform_.rotate.y = 0.0f;
      ChangeState(BossState::Battle);
    }
    break;
  }
  case BossState::Battle: {
    // ふわふわ浮かぶアニメーション
    static float time = 0.0f;
    time += 0.05f * speedMultiplier;
    transform_.translate.x = targetPos_.x;
    transform_.translate.z = targetPos_.z;
    transform_.translate.y = targetPos_.y + std::sin(time) * 1.0f;
    break;
  }
  case BossState::Defeat:
    // 撃破時の演出（落下して消えるなど）
    transform_.translate.y -= 0.1f * speedMultiplier;
    if (stateTimer_ > 3.0f) {
      isActive_ = false; // 演出が終わったら非アクティブに
    }
    break;

  case BossState::Victory:
    // ボス勝利時（プレイヤー敗北時）の演出（高笑い、飛び去るなど）
    transform_.translate.y += 0.1f * speedMultiplier;
    break;
  }

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
  
  const char* stateNames[] = { "Appearance", "Battle", "Defeat", "Victory" };
  ImGui::Text("Boss State: %s", stateNames[static_cast<int>(state_)]);
  ImGui::Text("State Timer: %.2f", stateTimer_);
  
  if (ImGui::Button("Test Defeat State")) ChangeState(BossState::Defeat);
  if (ImGui::Button("Test Victory State")) ChangeState(BossState::Victory);
}

void Boss::OnDamage() {
  if (state_ == BossState::Defeat) return; // 撃破後はダメージを受けない

  hp_--;
  isHit_ = true;
  hitTimer_ = 0.5f; // 0.5秒間ダメージ演出
  model_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 白く光る
  
  if (hp_ <= 0 && state_ == BossState::Battle) {
    ChangeState(BossState::Defeat);
  }
}
