#include "Obstacle.h"

Obstacle::Obstacle() {
  name_ = "Obstacle";
  isActive_ = false;
}

Obstacle::~Obstacle() {}

void Obstacle::Initialize(ModelData normalData, ModelData bonusData,
                          Type type) {
  normalModelData_ = normalData;
  bonusModelData_ = bonusData;
  SetType(type);
}

void Obstacle::SetType(Type type) {
  type_ = type;

  if (type_ == Type::Bonus) {
    model_->Initialize(bonusModelData_);
  } else {
    model_->Initialize(normalModelData_);
  }

  model_->SetShader("ObjectShader");

  // タイプに応じて当たり判定サイズとスケールを設定
  switch (type_) {
  case Type::Low:
    // ジャンプで避ける低い障害物
    collisionWidth_ = 1.5f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.5f, 1.0f, 1.0f};
    model_->GetMartial()->SetColor({0.0f, 1.0f, 0.0f, 1.0f}); // 緑色
    break;
  case Type::High:
    // 転がりで避ける高い障害物（上に浮いている）
    collisionWidth_ = 1.5f;
    collisionHeight_ = 3.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.5f, 3.0f, 1.0f};
    model_->GetMartial()->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
    break;
  case Type::Wall:
    // レーン移動で避ける壁
    collisionWidth_ = 1.5f;
    collisionHeight_ = 3.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.5f, 3.0f, 1.0f};
    model_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 白色
    model_->SetShader("IceShader");
    break;
  case Type::Bonus:
    // 当たると吹き飛ぶボーナスエネミー
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    model_->GetMartial()->SetColor({1.0f, 0.84f, 0.0f, 1.0f}); // 金色
    model_->SetShader("ObjectShader"); // IceShaderから戻す可能性があるため明示
    break;
  }

  model_->SetTransform(transform_);
}

void Obstacle::Spawn(float x, float y, float z) {
  transform_.translate = {x, y, z};
  isActive_ = true;
  isHit_ = false; // 初期化
  model_->SetTransform(transform_);
}

void Obstacle::StageUpdate(Matrix4x4 view, float scrollSpeed) {
  if (!isActive_)
    return;

  if (isHit_) {
    // 吹き飛び演出
    transform_.translate.x += velocity_.x;
    transform_.translate.y += velocity_.y;
    transform_.translate.z += velocity_.z;
    velocity_.y -= gravity_; // 重力

    transform_.rotate.x -= 0.15f; // 回転させる
    transform_.rotate.y += 0.1f;
    transform_.rotate.z += 0.05f;

    // カメラにぶつかる演出（カメラのZ座標付近に来たら）
    if (transform_.translate.z < -13.0f && velocity_.z < 0.0f) {
      velocity_.z = 0.0f;  // 手前に来るのを止める（画面に張り付いたような演出）
      velocity_.y = -0.1f; // 下に落ち始める
      velocity_.x = ((float)rand() / RAND_MAX - 0.5f) * 0.1f; // 横に少しずれる
    }

    if (transform_.translate.y < -20.0f) {
      isActive_ = false; // 画面外で消す
    }
  } else {
    // 手前にスクロール
    transform_.translate.z -= scrollSpeed;

    // カメラの後ろ（手前）を過ぎたら非アクティブにする
    if (transform_.translate.z < -10.0f) {
      isActive_ = false;
    }
  }

  model_.get()->SetTransform(transform_);
  model_.get()->SettingWvp(view);

  // コンポーネント（ColliderComponentなど）のUpdateを呼ぶ
  GameObject::Update(view, 1.0f);
}

void Obstacle::OnHit() {
  if (isHit_)
    return;
  isHit_ = true;
  // 上と手前(画面方向)に勢いよく飛ぶ
  float randX = ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
  velocity_ = {randX, 0.6f, -0.8f};
}

void Obstacle::Draw() {
  if (!isActive_) return;
  Draw::DrawObj(model_.get());
  GameObject::Draw();
}

void Obstacle::ImGuiInnerComponents() {
  if (model_) {
    model_->ImGui(false);
  }
}
