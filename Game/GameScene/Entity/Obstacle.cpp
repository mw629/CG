#include "Obstacle.h"
#include "Graphics/Render/Draw.h"

Obstacle::Obstacle() {
  name_ = "Obstacle";
  isActive_ = false;
}

Obstacle::~Obstacle() {}

void Obstacle::Initialize(ModelData lowData, ModelData highData,
                          ModelData wallData, ModelData bonusData,
                          ModelData iceBomData, ModelData reflectingAttackData,
                          Type type) {
  lowModelData_ = lowData;
  highModelData_ = highData;
  wallModelData_ = wallData;
  bonusModelData_ = bonusData;
  iceBomModelData_ = iceBomData;
  reflectingAttackModelData_ = reflectingAttackData;

  lowModel_->Initialize(lowData);
  lowModel_->SetShader("ObjectShader");
  lowModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

  highModel_->Initialize(highData);
  highModel_->SetShader("ObjectShader");
  highModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

  wallModel_->Initialize(wallData);
  wallModel_->SetShader("ObjectShader");
  wallModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

  bonusModel_->Initialize(bonusData);
  bonusModel_->SetShader("ObjectShader");
  bonusModel_->GetMartial()->SetColor({1.0f, 0.84f, 0.0f, 1.0f});

  itemModel_->Initialize(wallData);
  itemModel_->SetShader("ObjectShader");

  iceBomModel_->Initialize(iceBomData);
  iceBomModel_->SetShader("ObjectShader");
  iceBomModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

  reflectingAttackModel_->Initialize(reflectingAttackData);
  reflectingAttackModel_->SetShader("ObjectShader");
  reflectingAttackModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

  SetType(type);
}

void Obstacle::Initialize(ModelData lowData, ModelData highData,
                          ModelData wallData, ModelData bonusData,
                          Type type) {
  Initialize(lowData, highData, wallData, bonusData, wallData, wallData, type);
}

void Obstacle::Initialize(ModelData normalData, ModelData bonusData,
                          Type type) {
  Initialize(normalData, normalData, normalData, bonusData, type);
}

void Obstacle::SetType(Type type) {
  type_ = type;

  // タイプに応じてモデル、当たり判定サイズ、スケール、回転を設定
  // 前のサイズ: Low(幅1.5, 高さ1.0, 奥行1.0), High/Wall(幅1.5, 高さ3.0, 奥行1.0) に厳密に一致させる
  switch (type_) {
  case Type::Low:
    // 倒木（ジャンプで避ける低い障害物）
    currentModel_ = lowModel_.get();
    collisionWidth_ = 1.5f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    // FallenTree: ローカルX=0.712, Y=0.639, Z=0.999 をY90度回転して幅1.5, 高1.0, 奥1.0にする
    transform_.scale = {1.404f, 1.565f, 1.502f};
    transform_.rotate = {0.0f, 1.570796f, 0.0f}; // 横向きに倒れる
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
      currentModel_->SetBlend(BlendMode::kBlendModeNormal);
    }
    break;

  case Type::High:
    // 氷のアーチ（転がり・スライディングで下を潜り抜ける高い障害物）
    currentModel_ = highModel_.get();
    collisionWidth_ = 1.5f;
    collisionHeight_ = 3.0f;
    collisionDepth_ = 1.0f;
    // IceArchway: ローカルX=0.331, Y=0.939, Z=0.958 をY90度回転して幅1.5, 高3.0, 奥1.0にする
    transform_.scale = {3.021f, 3.195f, 1.566f};
    transform_.rotate = {0.0f, 1.570796f, 0.0f}; // 開口部をZ軸方向に向ける
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
      currentModel_->SetBlend(BlendMode::kBlendModeNormal);
    }
    break;

  case Type::Wall:
    // 氷の壁（レーン移動で避ける壁）
    currentModel_ = wallModel_.get();
    collisionWidth_ = 1.5f;
    collisionHeight_ = 3.0f;
    collisionDepth_ = 1.0f;
    // IceWall: ローカルX=1.002, Y=0.772, Z=0.319 を幅1.5, 高3.0, 奥1.0にする
    transform_.scale = {1.497f, 3.886f, 3.135f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
      currentModel_->SetBlend(BlendMode::kBlendModeNormal);
    }
    break;

  case Type::Bonus:
    // 当たると吹き飛ぶボーナスエネミー
    currentModel_ = bonusModel_.get();
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 0.84f, 0.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
      currentModel_->SetBlend(BlendMode::kBlendModeNormal);
    }
    break;

  case Type::GuideFloor:
    // 中央へ誘導するトリガー床
    currentModel_ = itemModel_.get();
    collisionWidth_ = 2.0f;
    collisionHeight_ = 0.5f;
    collisionDepth_ = 2.0f;
    transform_.scale = {2.0f, 0.1f, 10.0f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({0.0f, 1.0f, 1.0f, 0.5f});
      currentModel_->SetShader("ObjectShader");
    }
    break;

  case Type::CameraItem:
    currentModel_ = itemModel_.get();
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 0.0f, 1.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
    }
    break;

  case Type::BarrierItem:
    currentModel_ = itemModel_.get();
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({0.0f, 1.0f, 0.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
    }
    break;

  case Type::ClearItem:
    currentModel_ = itemModel_.get();
    collisionWidth_ = 1.0f;
    collisionHeight_ = 1.0f;
    collisionDepth_ = 1.0f;
    transform_.scale = {1.0f, 1.0f, 1.0f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 0.5f, 0.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
    }
    break;

  case Type::BossItem:
    currentModel_ = itemModel_.get();
    collisionWidth_ = 1.5f;
    collisionHeight_ = 1.5f;
    collisionDepth_ = 1.5f;
    transform_.scale = {1.5f, 1.5f, 1.5f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
    }
    break;

  case Type::BossAttack:
    // ボスの氷爆弾攻撃（IceBom / 回避専用）
    currentModel_ = iceBomModel_.get();
    collisionWidth_ = 1.6f;
    collisionHeight_ = 2.0f;
    collisionDepth_ = 1.6f;
    // IceBom: 約100x94x99 を幅約1.6, 高約1.5, 奥約1.6にする
    transform_.scale = {0.016f, 0.016f, 0.016f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
      currentModel_->SetBlend(BlendMode::kBlendModeNormal);
    }
    break;

  case Type::BossAttackReflectable:
    // ボスの跳ね返し可能攻撃（ReflectingAttack）
    currentModel_ = reflectingAttackModel_.get();
    collisionWidth_ = 1.6f;
    collisionHeight_ = 2.0f;
    collisionDepth_ = 2.5f;
    // ReflectingAttack: 約58x71x99 を幅約1.44, 高約1.78, 奥約2.49にする (+Zが前方)
    transform_.scale = {0.025f, 0.025f, 0.025f};
    transform_.rotate = {0.0f, 0.0f, 0.0f};
    if (currentModel_) {
      currentModel_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
      currentModel_->SetShader("ObjectShader");
      currentModel_->SetBlend(BlendMode::kBlendModeNormal);
    }
    break;
  }

  if (currentModel_) {
    // 新モデルは底面原点(Y=0)のため、中心座標から高さの半分を引いて底面を合わせる
    Transform drawTransform = transform_;
    if (currentModel_ == lowModel_.get() || currentModel_ == highModel_.get() ||
        currentModel_ == wallModel_.get() ||
        currentModel_ == iceBomModel_.get() ||
        currentModel_ == reflectingAttackModel_.get()) {
      drawTransform.translate.y -= collisionHeight_ * 0.5f;
    }
    currentModel_->SetTransform(drawTransform);
  }
}

void Obstacle::Spawn(float x, float y, float z) {
  targetY_ = y;
  if (type_ == Type::BossAttack || type_ == Type::BossAttackReflectable) {
    isFalling_ = true;
    fallTimer_ = 0.0f;
    justLanded_ = false;
    transform_.translate = {x, targetY_ + dropHeight_, z};
  } else {
    isFalling_ = false;
    justLanded_ = false;
    transform_.translate = {x, y, z};
  }
  isActive_ = true;
  isHit_ = false; // 初期化
  isReflected_ = false;
  if (currentModel_) {
    Transform drawTransform = transform_;
    if (currentModel_ == lowModel_.get() || currentModel_ == highModel_.get() ||
        currentModel_ == wallModel_.get() ||
        currentModel_ == iceBomModel_.get() ||
        currentModel_ == reflectingAttackModel_.get()) {
      drawTransform.translate.y -= collisionHeight_ * 0.5f;
    }
    currentModel_->SetTransform(drawTransform);
  }
}

void Obstacle::StageUpdate(Matrix4x4 view, float scrollSpeed) {
  if (!isActive_)
    return;

  // 前フレームの着地フラグをクリア
  justLanded_ = false;

  if (isHit_) {
    isFalling_ = false;
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
    isFalling_ = false;
    // ボス（ターゲット）へ向かって飛ぶ
    Vector3 dir = {reflectedTarget_.x - transform_.translate.x,
                   reflectedTarget_.y - transform_.translate.y,
                   reflectedTarget_.z - transform_.translate.z};
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

    if (len > 0.1f) {
      dir.x /= len;
      dir.y /= len;
      dir.z /= len;
      float speed = scrollSpeed * 5.0f; // 5倍の速度で飛んでいく
      if (speed > len)
        speed = len; // 行き過ぎ防止
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
      // 上空からの落下演出
      if (isFalling_) {
        fallTimer_ += 1.0f;
        float t = fallTimer_ / fallDuration_;
        if (t >= 1.0f) {
          t = 1.0f;
          isFalling_ = false;
          justLanded_ = true;
          transform_.translate.y = targetY_;
        } else {
          // Ease-In (重力加速): t * t
          float easeT = t * t;
          float startY = targetY_ + dropHeight_;
          transform_.translate.y = startY + (targetY_ - startY) * easeT;
        }
      }

      // ボスの攻撃は奥から手前(+Z方向)へ
      transform_.translate.z += scrollSpeed;
      if (transform_.translate.z > 20.0f) {
        isActive_ = false;
        isFalling_ = false;
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

  if (currentModel_) {
    Transform drawTransform = transform_;
    if (currentModel_ == lowModel_.get() || currentModel_ == highModel_.get() ||
        currentModel_ == wallModel_.get() ||
        currentModel_ == iceBomModel_.get() ||
        currentModel_ == reflectingAttackModel_.get()) {
      drawTransform.translate.y -= collisionHeight_ * 0.5f;
    }
    currentModel_->SetTransform(drawTransform);
    currentModel_->SettingWvp(view);
  }

  // コンポーネント（ColliderComponentなど）のUpdateを呼ぶ
  GameObject::Update(view, 1.0f);
}

void Obstacle::OnBlowAway() {
  if (isHit_)
    return;
  isHit_ = true;
  isFalling_ = false;
  // 上と手前(画面方向)に勢いよく飛ぶ
  float randX = ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
  velocity_ = {randX, 0.6f, -0.8f};
}

void Obstacle::OnHit() {
  if (isHit_)
    return;
  isHit_ = true;
  isFalling_ = false;
}

void Obstacle::Draw(class Draw &draw) {
  if (!isActive_ || !currentModel_)
    return;
  draw.DrawObj(currentModel_);
  GameObject::Draw(draw);
}

void Obstacle::ImGuiInnerComponents() {
  if (currentModel_) {
    currentModel_->ImGui(false);
  }
}
