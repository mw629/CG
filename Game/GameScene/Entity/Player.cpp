#include "Player.h"
#include "Graphics/Render/Draw.h"

Player::Player() {}

Player::~Player() {}

void Player::Initialize(ModelData modelData) {
  transform_.translate.y = baseHeight_;
  // Animationの初期化
  ModelData animModel =
      AssimpLoadObjFile("Resources/gltf/human", "sneakWalk.gltf");
  model_->Initialize(animModel, "Resources/gltf/human", "sneakWalk.gltf");
  model_->LoadAdditionalAnimation("Resources/gltf/human", "sneakWalk.gltf",
                                  "sneakWalk");
  model_->LoadAdditionalAnimation("Resources/gltf/human", "walk.gltf", "walk");
  model_->SetAnimation("walk");
  model_->name_ = "Player Animation Model";
  model_->SetVisibleBones(true);

  // 手のボーンを登録
  model_->SetBoneMapping(BoneType::RightHand, "mixamorig:RightHand");
  model_->SetBoneMapping(BoneType::LeftHand, "mixamorig:LeftHand");

  // Axeの初期化
  ModelData axeData = AssimpLoadObjFile("Resources/Model/Axe", "Axe.obj");
  axe_->Initialize(axeData);
  axe_->name_ = "Player Axe Model";
  axeOffset_.scale = {100.0f, 100.0f, 100.0f};
  axeOffset_.rotate = {0.0f, 0.0f, 0.0f};
  axeOffset_.translate = {0.0f, 0.0f, 0.0f};

  // パーティクルの初期化
  leftHandParticle_->Initialize();
  leftHandParticle_->LoadFromJson("particle1.json");
  leftHandParticle_->name_ = "Player LeftHand Particle";

  // コライダーの初期化
  auto collider = AddComponent<ColliderComponent>();
  collider->SetShape(ColliderShape::Box);
  collider->SetSize({0.8f, 1.5f, 0.8f});

  Transform drawTransform = transform_;
  drawTransform.scale.y = 1.0f;
  drawTransform.translate.y -= (isRolling_ ? 0.5f : 1.0f);
  model_->SetTransform(drawTransform);
}

void Player::Reset() {
  transform_.scale = {1.0f, 1.0f, 1.0f};
  transform_.rotate = {0.0f, 0.0f, 0.0f};
  transform_.translate = {0.0f, baseHeight_, 0.0f};

  laneIndex_ = 0;
  targetLaneIndex_ = 0;
  lerpTime_ = 0.0f;
  startX_ = 0.0f;
  moveDirection_ = MoveDirection::None;

  isJumping_ = false;
  velocityY_ = 0.0f;
  isRolling_ = false;
  rollTimer_ = 0.0f;
  keepRolling_ = false;
  currentRecoveryTimer_ = 0.0f;

  isHit_ = false;
  isTrip_ = false;
  hitTimer_ = 0.0f;
  knockbackVelocity_ = {0.0f, 0.0f, 0.0f};

  isForcedCentering_ = false;
  forcedCenterTimer_ = 0.0f;
  forcedCenterDuration_ = 30.0f;
  forcedCenterStartX_ = 0.0f;

  SetHasBarrier(false);

  Transform drawTransform = transform_;
  drawTransform.scale.y = 1.0f;
  drawTransform.translate.y -= (isRolling_ ? 0.5f : 1.0f);
  model_->SetTransform(drawTransform);
}

void Player::Update(Matrix4x4 view, float speedMultiplier) {
  if (isHit_) {
    // SpeedMultiplier is ignored for hit update so animation plays consistently
    // even if the game scroll stops.
    if (speedMultiplier > 0.0f) {
      HitUpdate(1.0f);
    } else {
      HitUpdate(0.0f);
    }
  } else {
    PlayerMove(speedMultiplier);
  }

  // アニメーション更新（走っている際）
  if (speedMultiplier > 0.0f) {
    model_->UpdateWithDelta(view, speedMultiplier * 0.01f);
  } else {
    model_->UpdateWithDelta(view, 0.0f);
  }

  model_->SettingWvp(view);

  // Axeのアタッチ処理
  Transform handTransform = model_->GetBoneTransform(BoneType::RightHand);
  Matrix4x4 boneMatrix = MakeAffineMatrix(
      handTransform.translate, handTransform.scale, handTransform.rotate);
  Matrix4x4 offsetMatrix = MakeAffineMatrix(
      axeOffset_.translate, axeOffset_.scale, axeOffset_.rotate);
  Matrix4x4 finalMatrix = MultiplyMatrix4x4(offsetMatrix, boneMatrix);
  axe_->SetTransform(DecomposeMatrix(finalMatrix));
  axe_->SettingWvp(view);

  // 左手パーティクルのアタッチ処理
  Transform leftHandTransform = model_->GetBoneTransform(BoneType::LeftHand);
  leftHandParticle_->SetPosition(leftHandTransform.translate);
  leftHandParticle_->Update(view);

  if (auto collider = GetComponent<ColliderComponent>()) {
    collider->SetSize({0.8f, isRolling_ ? 0.5f : 1.5f, 0.8f});
  }
  GameObject::Update(view, speedMultiplier);
}

void Player::PlayerMove(float speedMultiplier) {
  if (speedMultiplier <= 0.0f) {
    Transform drawTransform = transform_;
    drawTransform.scale.y = 1.0f;
    drawTransform.translate.y -= (isRolling_ ? 0.5f : 1.0f);
    model_->SetTransform(drawTransform);
    return;
  }

  // 硬直タイマーの更新
  if (currentRecoveryTimer_ > 0.0f) {
    currentRecoveryTimer_ -= speedMultiplier;
    if (currentRecoveryTimer_ < 0.0f) {
      currentRecoveryTimer_ = 0.0f;
    }
  }

  bool canAct = (currentRecoveryTimer_ <= 0.0f);

  // 強制中央移動の処理
  if (isForcedCentering_) {
    forcedCenterTimer_ += speedMultiplier;
    float t = forcedCenterTimer_ / forcedCenterDuration_;
    if (t > 1.0f) {
      t = 1.0f;
    }

    // スムーズな補間（EaseInOut）
    float easeT = t * t * (3.0f - 2.0f * t);
    transform_.translate.x = Lerp(forcedCenterStartX_, 0.0f, easeT);

    if (t >= 1.0f) {
      isForcedCentering_ = false;
      laneIndex_ = 0; // 中央レーンに確定
      targetLaneIndex_ = 0;
    }
  }
  // レーンの移動中ではなかったら（強制移動中でない時のみ入力受付）
  else if (laneIndex_ == targetLaneIndex_) {
    // キー入力で目標レーンを設定
    if (canAct) {
      if (Input::PushKey(DIK_A) || Input::PushKey(DIK_LEFT) ||
          GamePadInput::PushButton(XINPUT_GAMEPAD_DPAD_LEFT)) {
        targetLaneIndex_ = laneIndex_ - 1;
      }
      if (Input::PushKey(DIK_D) || Input::PushKey(DIK_RIGHT) ||
          GamePadInput::PushButton(XINPUT_GAMEPAD_DPAD_RIGHT)) {
        targetLaneIndex_ = laneIndex_ + 1;
      }
    }

    // レーンの範囲制限
    if (targetLaneIndex_ < minLane_)
      targetLaneIndex_ = minLane_;
    if (targetLaneIndex_ > maxLane_)
      targetLaneIndex_ = maxLane_;

    // 移動が開始される場合、初期値を保存
    if (targetLaneIndex_ != laneIndex_) {
      startX_ = transform_.translate.x;
      lerpTime_ = 0.0f;

      // 横移動時にしゃがみ（転がり）をキャンセルして硬直をなくす
      if (isRolling_ && !keepRolling_) {
        isRolling_ = false;
        transform_.scale.y = 1.0f;
        transform_.translate.y = baseHeight_;
        model_->SetAnimation("walk", 1.0f);
      }
    }
  }
  // レーンの移動中だったら
  else {
    // 線形補間で移動
    lerpTime_ += laneChangeSpeed_ * speedMultiplier;
    if (lerpTime_ > 1.0f) {
      lerpTime_ = 1.0f;
    }

    float targetX = static_cast<float>(targetLaneIndex_) * laneWidth_;
    transform_.translate.x = Lerp(startX_, targetX, lerpTime_);

    // 移動が完了したら現在のレーンを更新
    if (lerpTime_ >= 1.0f) {
      laneIndex_ = targetLaneIndex_;
      currentRecoveryTimer_ = laneChangeRecovery_;
    }
  }

  // === アクション（ジャンプと転がり） ===
  // 地上にいてジャンプ中でなければアクション可能（転がり中でもジャンプでキャンセル可能）
  if (!isJumping_ && canAct) {
    if (Input::PushKey(DIK_W) || Input::PushKey(DIK_SPACE) ||
        Input::PushKey(DIK_UP) ||
        GamePadInput::PushButton(XINPUT_GAMEPAD_DPAD_UP) ||
        GamePadInput::PushButton(XINPUT_GAMEPAD_A)) {
      if (!(isRolling_ && keepRolling_)) {
        isJumping_ = true;
        velocityY_ = jumpPower_ * speedMultiplier;

        // ジャンプ時にしゃがみをキャンセル
        if (isRolling_) {
          isRolling_ = false;
          transform_.scale.y = 1.0f;
          transform_.translate.y = baseHeight_;
          model_->SetAnimation("walk", 1.0f);
        }
      }
    } else if (!isRolling_ &&
               (Input::PushKey(DIK_S) || Input::PushKey(DIK_DOWN) ||
                GamePadInput::PushButton(XINPUT_GAMEPAD_DPAD_DOWN) ||
                GamePadInput::PushButton(XINPUT_GAMEPAD_B))) {
      isRolling_ = true;
      rollTimer_ = rollDuration_;
      model_->SetAnimation("sneakWalk", 1.0f);
      // 転がり中はスケールYを半分にして伏せるようにする
      transform_.scale.y = 0.5f;
      // 重心が変わる分、Y座標を少し下げる（原点が中心の場合）
      transform_.translate.y = baseHeight_ - 0.5f;
    }
  }

  // ジャンプ処理
  if (isJumping_) {
    transform_.translate.y += velocityY_;
    velocityY_ -= gravity_ * (speedMultiplier * speedMultiplier);

    // 地面に着地
    if (transform_.translate.y <= baseHeight_) {
      transform_.translate.y = baseHeight_;
      isJumping_ = false;
      velocityY_ = 0.0f;
      currentRecoveryTimer_ = jumpRecovery_;
    }
  }

  // 転がり処理
  if (isRolling_) {
    rollTimer_ -= speedMultiplier;
    if (rollTimer_ <= 0.0f && !keepRolling_) {
      isRolling_ = false;
      // 姿勢を元に戻す
      transform_.scale.y = 1.0f;
      transform_.translate.y = baseHeight_;
      model_->SetAnimation("walk", 1.0f);
      currentRecoveryTimer_ = rollRecovery_;
    }
  }

  // トランスフォームをモデルに適用
  Transform drawTransform = transform_;
  drawTransform.scale.y = 1.0f;
  drawTransform.translate.y -= (isRolling_ ? 0.5f : 1.0f);
  model_->SetTransform(drawTransform);
}

void Player::Draw(class Draw &draw) {
  draw.DrawAnimation(model_.get());
  draw.DrawModel(axe_.get());
  GameObject::Draw(draw);
}

void Player::DrawParticle(class Draw &draw) {
  leftHandParticle_->Draw(draw);
}

void Player::ImGuiInnerComponents() {
  if (model_) {
    model_->ImGui(false);
  }
}

void Player::HitUpdate(float speedMultiplier) {
  // ノックバック処理
  if (isHit_) {
    if (speedMultiplier == 0.0f) {
      Transform drawTransform = transform_;
      drawTransform.scale.y = 1.0f;
      drawTransform.translate.y -= (isRolling_ ? 0.5f : 1.0f);
      model_->SetTransform(drawTransform);
      return;
    }

    hitTimer_ += 1.0f;

    transform_.translate.x += knockbackVelocity_.x;
    transform_.translate.y += knockbackVelocity_.y;
    transform_.translate.z += knockbackVelocity_.z;

    // 重力と回転（後ろに飛ぶか前に転がるか）
    knockbackVelocity_.y -= gravity_ * 2.0f;
    if (isTrip_) {
      transform_.rotate.x += 0.2f; // 前に転がる回転
    } else {
      transform_.rotate.x -= 0.1f; // 後ろに飛ぶ回転
    }

    // 地面に着地したらバウンドなどを抑える
    if (transform_.translate.y <= baseHeight_ && knockbackVelocity_.y < 0.0f) {
      transform_.translate.y = baseHeight_;
      knockbackVelocity_.y = 0.0f;
      knockbackVelocity_.x *= 0.8f;
      knockbackVelocity_.z *= 0.8f;
    }

    Transform drawTransform = transform_;
    drawTransform.scale.y = 1.0f;
    drawTransform.translate.y -= (isRolling_ ? 0.5f : 1.0f);
    model_->SetTransform(drawTransform);
  }
}

void Player::OnHit(bool isTrip) {
  isHit_ = true;
  isTrip_ = isTrip;
  hitTimer_ = 0.0f;

  // 姿勢をリセット
  isRolling_ = false;
  isJumping_ = false;
  transform_.scale = {1.0f, 1.0f, 1.0f};
  model_->SetAnimation("walk", 1.0f);
  currentRecoveryTimer_ = 0.0f;

  float randX = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;

  if (isTrip_) {
    // Lowに当たってつまずいた場合、少し前に転がるようなノックバック
    knockbackVelocity_ = {randX, 0.3f, 0.8f};
  } else {
    // 少し後ろと上に飛ぶノックバック
    knockbackVelocity_ = {randX, 0.4f, -0.6f};
  }
}

bool Player::IsHitAnimationFinished() const {
  return isHit_ && hitTimer_ >= hitDuration_;
}

void Player::StartForceToCenter(float duration) {
  if (isForcedCentering_ || targetLaneIndex_ == 0) {
    return; // 既に移動中、または既に中央目標の場合は何もしない
  }
  isForcedCentering_ = true;
  forcedCenterTimer_ = 0.0f;
  forcedCenterDuration_ = duration;
  forcedCenterStartX_ = transform_.translate.x;

  // 転がり中なら解除する（安全のため）
  if (isRolling_ && !keepRolling_) {
    isRolling_ = false;
    transform_.scale.y = 1.0f;
    transform_.translate.y = baseHeight_;
    model_->SetAnimation("walk", 1.0f);
  }
}

void Player::SetHasBarrier(bool hasBarrier) {
  hasBarrier_ = hasBarrier;
  if (model_ && model_->GetMartial()) {
    if (hasBarrier_) {
      model_->GetMartial()->SetColor({0.0f, 1.0f, 1.0f, 1.0f}); // シアン
    } else {
      model_->GetMartial()->SetColor(
          {1.0f, 1.0f, 1.0f, 1.0f}); // 白（デフォルト）
    }
  }
}
