#include "Obstacle.h"
#include "Graphics/Render/Draw.h"

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
    model_->GetMartial()->SetColor({0.0f, 0.0f, 1.0f, 1.0f}); // 緑色
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
  case Type::GuideFloor:
    // 中央へ誘導するトリガー床（床に埋め込むか薄くする）
    collisionWidth_ = 2.0f;
    collisionHeight_ = 0.5f;
    collisionDepth_ = 2.0f;
    transform_.scale = {2.0f, 0.1f,
                        10.0f}; // 縦長にして光るレールのように見せる
    model_->GetMartial()->SetColor({0.0f, 1.0f, 1.0f, 0.5f}); // シアン・半透明
    model_->SetShader("ObjectShader");
    break;
  case Type::CameraItem:
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    model_->GetMartial()->SetColor({1.0f, 0.0f, 1.0f, 1.0f}); // マゼンタ
    model_->SetShader("ObjectShader");
    break;
  case Type::BarrierItem:
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    model_->GetMartial()->SetColor({0.0f, 1.0f, 0.0f, 1.0f}); // 緑（バリア）
    model_->SetShader("ObjectShader");
    break;
  case Type::ClearItem:
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    model_->GetMartial()->SetColor(
        {1.0f, 0.5f, 0.0f, 1.0f}); // オレンジ（ボム/クリア）
    model_->SetShader("ObjectShader");
    break;
  case Type::BossItem:
    collisionWidth_ = 1.5f;
    collisionHeight_ = 1.5f;
    collisionDepth_ = 1.5f;
    transform_.scale = {1.5f, 1.5f, 1.5f};
    model_->GetMartial()->SetColor({1.0f, 0.0f, 0.0f, 1.0f}); // 赤（ボス）
    model_->SetShader("ObjectShader");
    break;
  case Type::BossAttack:
    collisionWidth_ = 1.5f;
    collisionHeight_ = 3.0f; // 飛び越え不可
    collisionDepth_ = 1.0f;
    transform_.scale = {1.5f, 3.0f, 1.0f};
    model_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 白
    model_->SetShader("IceShader");
    break;
  case Type::BossAttackReflectable:
    collisionWidth_ = 1.5f;
    collisionHeight_ = 3.0f; // 飛び越え不可
    collisionDepth_ = 1.0f;
    transform_.scale = {1.5f, 3.0f, 1.0f};
    model_->GetMartial()->SetColor({0.0f, 1.0f, 0.0f, 1.0f}); // 緑
    model_->SetShader("IceShader");
    break;
  }


  model_->SetTransform(transform_);
}

void Obstacle::Spawn(float x, float y, float z) {
  transform_.translate = {x, y, z};
  isActive_ = true;
  isHit_ = false; // 初期化
  isReflected_ = false;
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
  } else if (isReflected_) {
    // ボス（ターゲット）へ向かって飛ぶ
    Vector3 dir = {
      reflectedTarget_.x - transform_.translate.x,
      reflectedTarget_.y - transform_.translate.y,
      reflectedTarget_.z - transform_.translate.z
    };
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    
    if (len > 0.1f) {
      dir.x /= len; dir.y /= len; dir.z /= len;
      float speed = scrollSpeed * 5.0f; // 5倍の速度で飛んでいく
      if (speed > len) speed = len; // 行き過ぎ防止
      transform_.translate.x += dir.x * speed;
      transform_.translate.y += dir.y * speed;
      transform_.translate.z += dir.z * speed;
    }
    
    // 回転させながら飛ぶ
    transform_.rotate.x -= 0.3f;
    transform_.rotate.y += 0.2f;
  } else {
    // スクロール
    if (type_ == Type::BossAttack || type_ == Type::BossAttackReflectable) {
      // ボスの攻撃は奥から手前(+Z方向)へ
      transform_.translate.z += scrollSpeed;
      if (transform_.translate.z > 20.0f) {
        isActive_ = false;
      }
    } else {
      // 手前にスクロール
      transform_.translate.z -= scrollSpeed;

      // カメラの後ろ（手前）を過ぎたら非アクティブにする
      if (transform_.translate.z < -10.0f) {
        isActive_ = false;
      }
    }
  }


  model_.get()->SetTransform(transform_);
  model_.get()->SettingWvp(view);

  // コンポーネント（ColliderComponentなど）のUpdateを呼ぶ
  GameObject::Update(view, 1.0f);
}

void Obstacle::OnBlowAway() {
  if (isHit_)
    return;
  isHit_ = true;
  // 上と手前(画面方向)に勢いよく飛ぶ
  float randX = ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
  velocity_ = {randX, 0.6f, -0.8f};
}

void Obstacle::OnHit() {
  if (isHit_)
    return;
  isHit_ = true;
}

void Obstacle::Draw(class Draw &draw) {
  if (!isActive_)
    return;
  draw.DrawObj(model_.get());
  GameObject::Draw(draw);
}

void Obstacle::ImGuiInnerComponents() {
  if (model_) {
    model_->ImGui(false);
  }
}
