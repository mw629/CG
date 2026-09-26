#include "GameScene.h"
#include "../../Editer/EditorManager.h"
#include "AssetManager.h"
#include "Graphics/Render/Draw.h"
#include <Engine.h>
#include <GameObjects/Object/3d/Model.h>
#include <Math/Calculation.h>
#include <System/CollisionManager.h>
#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <memory>

GameScene::~GameScene() {
  EditorManager::SetGameViewDrawCallback(nullptr);
  EditorManager::SetSaveCallback(nullptr);
  EditorManager::SetLoadCallback(nullptr);
  EditorManager::SetFileDropCallback(nullptr);
  EditorManager::ClearSceneOverlayCallback();
}

void GameScene::ImGui() {
#ifdef _USE_IMGUI
  ImGui::Begin("GameScene");

  // ゲームオーバー時の表示
  if (gameState_ == GameState::GameOver) {
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.8f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
    bool open = ImGui::CollapsingHeader("=== GAME OVER ===",
                                        ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::PopStyleColor(2);

    if (open) {
      ImGui::Text("Your Distance: %.2f m", currentDistance_);
      ImGui::Text("Your Score: %.0f", currentScore_);
      ImGui::Separator();

      ImGui::Text("--- DISTANCE TOP 3 RANKING ---");
      for (int i = 0; i < 3; i++) {
        if (topRankings_[i] > 0.0f) {
          ImGui::Text("  %d. %.2f m", i + 1, topRankings_[i]);
        } else {
          ImGui::Text("  %d. ---", i + 1);
        }
      }
      ImGui::Separator();

      ImGui::Text("--- SCORE TOP 3 RANKING ---");
      for (int i = 0; i < 3; i++) {
        if (topScoreRankings_[i] > 0.0f) {
          ImGui::Text("  %d. %.0f", i + 1, topScoreRankings_[i]);
        } else {
          ImGui::Text("  %d. ---", i + 1);
        }
      }
      ImGui::Separator();

      if (ImGui::Button("Restart (1)", ImVec2(160, 35))) {
        ResetGame();
        gameState_ = GameState::Playing;
      }
      ImGui::SameLine();
      if (ImGui::Button("Return to Title (2)", ImVec2(160, 35))) {
        ResetGame();
        gameState_ = GameState::Title;
      }
      ImGui::Separator();
    }
  }

  camera_.get()->ImGui();

  if (ImGui::CollapsingHeader("GameScene Camera Settings",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    bool cameraChanged = false;
    if (ImGui::DragFloat3("Translate", &cameraTransform_.translate.x, 0.1f))
      cameraChanged = true;
    if (ImGui::DragFloat3("Rotate", &cameraTransform_.rotate.x, 0.01f))
      cameraChanged = true;
    if (ImGui::DragFloat3("Scale", &cameraTransform_.scale.x, 0.01f))
      cameraChanged = true;

    if (cameraChanged) {
      camera_->SetTransform(cameraTransform_);
      gameCamera_->SetTransform(cameraTransform_);
    }

    ImGui::Separator();
    ImGui::Text("Presets:");
    if (ImGui::Button("Behind View")) {
      ChangePlayingState(PlayingState::ThreeLane);
    }
    ImGui::SameLine();
    if (ImGui::Button("Left Side View")) {
      cameraTransform_.scale = {1.0f, 1.0f, 1.0f};
      cameraTransform_.rotate = {0.3f, 1.0472f, 0.0f};
      cameraTransform_.translate = {-20.0f, 8.0f, -5.0f};
      camera_->SetTransform(cameraTransform_);
      gameCamera_->SetTransform(cameraTransform_);
    }
    ImGui::SameLine();
    if (ImGui::Button("Right Side View")) {
      ChangePlayingState(PlayingState::OneLane);
    }

    ImGui::Separator();
    if (ImGui::Button("Reset Debug Camera to Game Camera")) {
      camera_->ResetDebugCamera(cameraTransform_);
    }
  }

  if (ImGui::CollapsingHeader("Game State", ImGuiTreeNodeFlags_DefaultOpen)) {
    const char *stateNames[] = {"Title",     "Playing",  "Paused", "PlayerHit",
                                "GameClear", "GameOver", "Editor"};
    ImGui::Text("Game State: %s", stateNames[gameState_]);

    const char *playingStateNames[] = {"ThreeLane", "OneLane", "Boss"};
    int currentStateIdx = static_cast<int>(playingState_);
    if (ImGui::Combo("Playing State", &currentStateIdx, playingStateNames, 3)) {
      PlayingState newState = static_cast<PlayingState>(currentStateIdx);
      ChangePlayingState(newState);
    }

    ImGui::Separator();

    // プレイヤー情報
    if (ImGui::TreeNode("Player Info")) {
      const Transform &playerTransform = player_->GetTransform();
      ImGui::Text("Position: (%.2f, %.2f, %.2f)", playerTransform.translate.x,
                  playerTransform.translate.y, playerTransform.translate.z);
      ImGui::Text("Rolling: %s", player_->GetIsRolling() ? "YES" : "NO");
      ImGui::Text("Jumping: %s", player_->GetIsJumping() ? "YES" : "NO");

      float jumpPower = player_->GetJumpPower();
      if (ImGui::SliderFloat("Jump Power", &jumpPower, 0.10f, 0.40f, "%.3f")) {
        player_->SetJumpPower(jumpPower);
      }
      float gravity = player_->GetGravity();
      if (ImGui::SliderFloat("Gravity", &gravity, 0.005f, 0.040f, "%.4f")) {
        player_->SetGravity(gravity);
      }
      float laneSpeed = player_->GetLaneChangeSpeed();
      if (ImGui::SliderFloat("Lane Speed", &laneSpeed, 0.05f, 0.50f, "%.2f")) {
        player_->SetLaneChangeSpeed(laneSpeed);
      }
      float rollDuration = player_->GetRollDuration();
      if (ImGui::SliderFloat("Roll Duration", &rollDuration, 10.0f, 60.0f,
                             "%.0f f")) {
        player_->SetRollDuration(rollDuration);
      }

      float estAirFrames =
          gravity > 0.0f ? ((2.0f * jumpPower / gravity) + 1.0f) : 0.0f;
      float estMaxHeight =
          gravity > 0.0f ? ((jumpPower * jumpPower) / (2.0f * gravity)) : 0.0f;
      ImGui::Text("Jump Air Time: %.0f frames (%.2f s)", estAirFrames,
                  estAirFrames / 60.0f);
      ImGui::Text("Max Jump Height: +%.2f m", estMaxHeight);
      ImGui::Text("Lane Move Time: %.0f frames",
                  laneSpeed > 0.0f ? (1.0f / laneSpeed) : 0.0f);
      ImGui::Text("Roll Duration: %.0f frames (%.2f s)", rollDuration,
                  rollDuration / 60.0f);

      ImGui::TreePop();
    }

    // ボス情報
    if (playingState_ == PlayingState::Boss && boss_->GetIsActive()) {
      if (ImGui::TreeNode("Boss Info")) {
        ImGui::Text("HP: %d / 20", boss_->GetHP());
        ImGui::DragFloat3("Target Pos", &boss_->GetTargetPosRef().x, 0.1f);
        ImGui::DragFloat("Attack Spawn Z", &bossAttackSpawnZ_, 0.5f, -60.0f, 0.0f, "%.1f m");
        ImGui::DragFloat("Attack Drop Height", &bossAttackDropHeight_, 0.5f, 5.0f, 35.0f, "%.1f m");
        ImGui::DragFloat("Attack Fall Frames", &bossAttackFallDuration_, 1.0f, 5.0f, 60.0f, "%.0f frames");
        ImGui::Text("Attack Distance to Player: %.1f m", -bossAttackSpawnZ_);
        ImGui::TreePop();
      }
    }

    // ステージ情報
    if (ImGui::TreeNode("Stage Info")) {
      ImGui::Text("Scroll Speed: %.3f", stageSettings_->GetScrollSpeed());
      ImGui::Text("Base Scroll Speed: %.3f",
                  stageSettings_->GetBaseScrollSpeed());
      ImGui::Text("Max Scroll Speed: %.3f",
                  stageSettings_->GetMaxScrollSpeed());
      ImGui::Text("Lane Index: Min=%d, Max=%d",
                  stageSettings_->GetMinLaneIndex(),
                  stageSettings_->GetMaxLaneIndex());
      ImGui::Text("Lane Width: %.2f (Effective: %.2f)",
                  stageSettings_->GetLaneWidth(),
                  stageSettings_->GetEffectiveLaneWidth());
      ImGui::Text("Chunks: %d (Behind: %d, Ahead: %d)",
                  stageSettings_->GetChunkCount(),
                  stageSettings_->GetBackwardChunks(),
                  stageSettings_->GetForwardChunks());
      ImGui::TreePop();
    }

    ImGui::Separator();

    // スコアとランキング
    ImGui::Text("Current Distance: %.2f m", currentDistance_);
    if (ImGui::TreeNode("Distance Top 3 Ranking")) {
      for (int i = 0; i < 3; i++) {
        if (topRankings_[i] > 0.0f) {
          ImGui::Text("Rank %d: %.2f m", i + 1, topRankings_[i]);
        } else {
          ImGui::Text("Rank %d: ---", i + 1);
        }
      }
      ImGui::TreePop();
    }

    ImGui::Text("Current Score: %.0f", currentScore_);
    if (ImGui::TreeNode("Score Top 3 Ranking")) {
      for (int i = 0; i < 3; i++) {
        if (topScoreRankings_[i] > 0.0f) {
          ImGui::Text("Rank %d: %.0f", i + 1, topScoreRankings_[i]);
        } else {
          ImGui::Text("Rank %d: ---", i + 1);
        }
      }
      ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::Button("Reset Game", ImVec2(120, 0))) {
      ResetGame();
      gameState_ = GameState::Playing;
      camera_->SetDebugCamera(false);
    }
    ImGui::SameLine();
    if (ImGui::Button("Go to Title", ImVec2(120, 0))) {
      ResetGame();
      gameState_ = GameState::Title;
      camera_->SetDebugCamera(false);
    }
  }

  // フォント / 文字の太さ設定パネル
  if (draw_ && ImGui::CollapsingHeader("Font / Text Settings")) {
    float boldness = draw_->GetTextBaseBoldness();
    if (ImGui::SliderFloat("Global Boldness", &boldness, -0.05f, 0.20f,
                           "%.3f")) {
      draw_->SetTextBaseBoldness(boldness);
    }
    ImGui::TextDisabled("Default: 0.070 (Thick / Bold)");
  }

  // ステージ設定のデバッグパネル
  if (ImGui::CollapsingHeader("Stage Settings Debug")) {
    int laneCount = stageSettings_->GetLaneCount();
    if (ImGui::SliderInt("Lane Count", &laneCount, 1, 11)) {
      // Ensure it's preferably an odd number, or just pass it to the setter
      stageSettings_->SetLaneCount(laneCount);
    }

    float laneWidth = stageSettings_->GetLaneWidth();
    if (ImGui::SliderFloat("Lane Width", &laneWidth, 1.0f, 10.0f)) {
      stageSettings_->SetLaneWidth(laneWidth);
    }

    float oneLaneMult = stageSettings_->GetOneLaneWidthMultiplier();
    if (ImGui::SliderFloat("One Lane Width Multiplier", &oneLaneMult, 1.0f,
                           3.0f, "%.2f")) {
      stageSettings_->SetOneLaneWidthMultiplier(oneLaneMult);
    }

    float baseSpeed = stageSettings_->GetBaseScrollSpeed();
    if (ImGui::SliderFloat("Base Scroll Speed", &baseSpeed, 0.0f, 1.0f)) {
      stageSettings_->SetBaseScrollSpeed(baseSpeed);
    }

    float maxSpeed = stageSettings_->GetMaxScrollSpeed();
    if (ImGui::SliderFloat("Max Scroll Speed", &maxSpeed, baseSpeed, 2.0f)) {
      stageSettings_->SetMaxScrollSpeed(maxSpeed);
    }

    float accel = stageSettings_->GetScrollAcceleration();
    if (ImGui::SliderFloat("Scroll Acceleration", &accel, 0.0f, 0.01f)) {
      stageSettings_->SetScrollAcceleration(accel);
    }

    float minDistance =
        stageSettings_
            ->GetMinObstacleDistance(); // 確実に避けられる最小間隔距離
    float maxDistance =
        stageSettings_->GetMaxObstacleDistance(); // 避けられる最大間隔距離
    if (ImGui::SliderFloat("Min Obstacle Distance", &minDistance, 3.0f, 30.0f,
                           "%.1f m")) {
      if (minDistance > maxDistance)
        maxDistance = minDistance;
      stageSettings_->SetMinObstacleDistance(minDistance);
      stageSettings_->SetMaxObstacleDistance(maxDistance);
    }
    if (ImGui::SliderFloat("Max Obstacle Distance", &maxDistance, 5.0f, 60.0f,
                           "%.1f m")) {
      if (maxDistance < minDistance)
        minDistance = maxDistance;
      stageSettings_->SetMinObstacleDistance(minDistance);
      stageSettings_->SetMaxObstacleDistance(maxDistance);
    }

    float baseActionFrames =
        stageSettings_->GetBaseActionFrames(); // 回避アクション所要フレーム数
    if (ImGui::SliderFloat("Base Action Frames", &baseActionFrames, 0.0f, 60.0f,
                           "%.0f f")) {
      stageSettings_->SetBaseActionFrames(baseActionFrames);
    }

    float minGrace = stageSettings_->GetMinGraceFrames(); // 最小猶予フレーム数
    float maxGrace = stageSettings_->GetMaxGraceFrames(); // 最大猶予フレーム数
    if (ImGui::SliderFloat("Min Grace Frames", &minGrace, 5.0f, 60.0f,
                           "%.0f f")) {
      if (minGrace > maxGrace)
        maxGrace = minGrace;
      stageSettings_->SetMinGraceFrames(minGrace);
      stageSettings_->SetMaxGraceFrames(maxGrace);
    }
    if (ImGui::SliderFloat("Max Grace Frames", &maxGrace, 5.0f, 80.0f,
                           "%.0f f")) {
      if (maxGrace < minGrace)
        minGrace = maxGrace;
      stageSettings_->SetMinGraceFrames(minGrace);
      stageSettings_->SetMaxGraceFrames(maxGrace);
    }

    float noSpawnChance =
        stageSettings_->GetNoSpawnChance() * 100.0f; // 障害物が出ない確率（%）
    if (ImGui::SliderFloat("No Spawn Chance (%)", &noSpawnChance, 0.0f, 30.0f,
                           "%.1f %%")) {
      stageSettings_->SetNoSpawnChance(noSpawnChance / 100.0f);
    }

    float currentSpeed =
        stageSettings_->GetScrollSpeed(); // 現在のスクロール速度
    float curInterval =
        stageSettings_->GetObstacleInterval(); // 現在の次回生成間隔
    float estFrames = currentSpeed > 0.0f ? (curInterval / currentSpeed)
                                          : 0.0f; // 到達までの推定フレーム数
    ImGui::Text("Next Spawn: %.1f m (approx. %.0f frames / %.2f s)",
                curInterval, estFrames, estFrames / 60.0f);
    ImGui::Text("Dodgeable Range: %.1f m ~ %.1f m", minDistance, maxDistance);
  }

  // アイテムクールタイム設定パネル
  if (ImGui::CollapsingHeader("Item CoolDowns (アイテムクールタイム設定)",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    float spawnChance = stageSettings_->GetItemSpawnChance() * 100.0f;
    if (ImGui::SliderFloat("Item Spawn Chance (%)", &spawnChance, 0.0f, 100.0f,
                           "%.1f %%")) {
      stageSettings_->SetItemSpawnChance(spawnChance / 100.0f);
    }
    ImGui::Separator();

    struct ItemInfo {
      Obstacle::Type type;
      const char *name;
      const char *desc;
    };
    ItemInfo items[] = {
        {Obstacle::Type::Bonus, "Bonus (ボーナス)", "スコア加算"},
        {Obstacle::Type::BarrierItem, "Barrier (バリア)", "ミスを1回防御"},
        {Obstacle::Type::ClearItem, "Clear (障害物全消去)",
         "画面内の障害物を一掃"},
        {Obstacle::Type::CameraItem, "Camera (視点切替)",
         "1レーンモードへ移行"},
        {Obstacle::Type::BossItem, "Boss (ボス戦突入)", "ボスバトル開始"},
    };

    for (const auto &item : items) {
      ImGui::PushID(static_cast<int>(item.type));
      float duration = stageSettings_->GetItemCoolDownDuration(item.type);
      float current = stageSettings_->GetItemCoolDownTimer(item.type);

      ImGui::Text("%s - %s", item.name, item.desc);
      if (ImGui::SliderFloat("CoolTime (s)", &duration, 0.0f, 120.0f,
                             "%.1f s")) {
        stageSettings_->SetItemCoolDownDuration(item.type, duration);
      }

      if (current <= 0.0f) {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f),
                           "  Status: READY (出現可能)");
      } else {
        ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f),
                           "  Status: COOLDOWN (残り %.1f s)", current);
      }

      ImGui::SameLine();
      if (ImGui::SmallButton("Reset CD (即可能)")) {
        stageSettings_->SetItemCoolDownTimer(item.type, 0.0f);
      }
      ImGui::SameLine();
      if (ImGui::SmallButton("Trigger CD")) {
        stageSettings_->SetItemCoolDownTimer(item.type, duration);
      }
      ImGui::Separator();
      ImGui::PopID();
    }
  }

  // アイテム効果のデバッグパネル
  if (ImGui::CollapsingHeader("Item Debug / Effects",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    bool hasBarrier = player_->GetHasBarrier();
    ImGui::Text("Player Barrier Status: %s",
                hasBarrier ? "ACTIVE (ON)" : "INACTIVE (OFF)");

    if (ImGui::Button("Give Barrier (バリア付与)")) {
      player_->SetHasBarrier(true);
      effectManager_->EmitBarrier(player_->GetTransform().translate);
      effectManager_->EmitShockwave(player_->GetTransform().translate);
    }
    ImGui::SameLine();
    if (ImGui::Button("Break Barrier (バリア破壊/解除)")) {
      player_->SetHasBarrier(false);
      effectManager_->BreakBarrier(player_->GetTransform().translate);
    }

    ImGui::Separator();
    if (ImGui::Button("Clear All Obstacles (全障害物吹き飛ばし)")) {
      effectManager_->EmitShockwave(player_->GetTransform().translate);
      for (int j = 0; j < stageSettings_->GetMaxObstacles(); j++) {
        Obstacle *obs = stageSettings_->GetObstacle(j);
        if (obs && obs->GetIsActive() &&
            (obs->GetType() == Obstacle::Type::Low ||
             obs->GetType() == Obstacle::Type::High ||
             obs->GetType() == Obstacle::Type::Wall)) {
          obs->OnBlowAway();
        }
      }
    }

    if (ImGui::Button("Trigger Camera Item (視点切り替え)")) {
      ChangePlayingState(PlayingState::OneLane);
      effectManager_->EmitShockwave(player_->GetTransform().translate);
    }
    ImGui::SameLine();
    if (ImGui::Button("Trigger Boss Item (ボス戦移行)")) {
      ChangePlayingState(PlayingState::Boss);
      effectManager_->EmitShockwave(player_->GetTransform().translate);
    }

    ImGui::Separator();
    ImGui::Text("Effect Triggers:");
    if (ImGui::Button("Emit Dust")) {
      effectManager_->EmitDust(player_->GetTransform().translate);
    }
    ImGui::SameLine();
    if (ImGui::Button("Emit Shockwave")) {
      effectManager_->EmitShockwave(player_->GetTransform().translate);
    }
    ImGui::SameLine();
    if (ImGui::Button("Emit Hit Spark")) {
      effectManager_->EmitHitEffect(player_->GetTransform().translate);
    }
  }

  effectManager_->ImGui();

  ImGui::End();

  Matrix4x4 projection = MakePerspectiveFovMatrix(
      0.45f, float(1280.0f) / float(720.0f), 0.1f, 10000.0f);
  editorUI_->Draw(gameObjectManager_.get(), view, projection);

  // Stopモードの時だけギズモ描画コールバックをSceneウィンドウに登録する
#ifdef _USE_IMGUI
  if (!EditorManager::IsPlaying()) {
    EditorManager::SetSceneOverlayCallback([this]() {
      Matrix4x4 proj = MakePerspectiveFovMatrix(
          0.45f, float(1280.0f) / float(720.0f), 0.1f, 10000.0f);
      editorUI_->DrawGizmoInScene(view, proj);
    });
  } else {
    EditorManager::ClearSceneOverlayCallback();
  }
#endif

  // ポーズメニュー
  if (gameState_ == GameState::Paused) {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Pause Menu", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoCollapse);
    ImGui::Text("PAUSED");
    ImGui::Separator();
    if (ImGui::Button("Resume (ESC)", ImVec2(200, 40))) {
      gameState_ = GameState::Playing;
    }
    if (ImGui::Button("Return to Title", ImVec2(200, 40))) {
      ResetGame();
      gameState_ = GameState::Title;
    }
    ImGui::End();
  }

  // タイトル中のImGui
  if (gameState_ == GameState::Title) {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.85f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Title Menu", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoCollapse);
    if (ImGui::Button("Start Game (SPACE)", ImVec2(220, 45))) {
      ResetGame();
      gameState_ = GameState::Playing;
    }
    ImGui::End();
  }

#endif // _USE_IMGUI
}

void GameScene::ResetGame() {
  stageSettings_->Reset();
  player_->Reset();
  effectManager_->ClearHitParticles();
  effectManager_->ClearBarrier();
  currentDistance_ = 0.0f;
  currentScore_ = 0.0f;
  bonusEnemyHitCount_ = 0;
  ChangePlayingState(PlayingState::ThreeLane, true);
}

void GameScene::Initialize() {

  sceneID_ = SceneID::Game;

  // タイトル用スプライトの生成
  titleTextureHandle_ = texture_->CreateTexture("Resources/Texture/Title.png");
  titleSpriteData_.transform.scale = {1.0f, 1.0f, 1.0f};
  titleSpriteData_.transform.translate = {640.0f, 360.0f, 0.0f};
  titleSpriteData_.transform.rotate = {0.0f, 0.0f, 0.0f};
  titleSpriteData_.size = {1280.0f, 720.0f};
  titleSpriteData_.textureArea[0] = {0.0f, 0.0f};
  titleSpriteData_.textureArea[1] = {1.0f, 1.0f};
  titleSprite_ = std::make_unique<Sprite>();
  titleSprite_->Initialize(titleSpriteData_, titleTextureHandle_);
  titleSprite_->GetMartial()->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

  if (!EditorManager::IsPlaying()) {
    gameState_ = GameState::Editor;
    camera_->SetDebugCamera(true);
  } else {
    gameState_ = GameState::Title;
    camera_->SetDebugCamera(false);
  }
  sceneChangeRequest_ = false;

  // パーティクルマネージャーの初期化
  effectManager_->Initialize();

  // コリジョンマネージャーの初期化
  collisionManager_ = std::make_unique<CollisionManager>();

  // camera_->SetDebugCamera() は上記で設定済み
  camera_->SetTransform(cameraTransform_);
  camera_->Update();

  gameCamera_->SetDebugCamera(
      false); // Game Cameraは常にDebug操作を受け付けない
  gameCamera_->SetTransform(cameraTransform_);
  gameCamera_->Update();

  // Game View描画コールバックの登録
  EditorManager::SetGameViewDrawCallback([this](class Draw &draw) {
    Matrix4x4 gameViewMat = gameCamera_->GetViewMatrix();

    // 一時的にSkyBoxをGameCameraの位置へ移動
    Transform originalSkyBoxT = skyBox_->GetTransform();
    Transform gameSkyBoxT = originalSkyBoxT;
    gameSkyBoxT.translate = gameCamera_->GetTransform().translate;
    skyBox_->SetTransform(gameSkyBoxT);

    // ゲームカメラのView行列でオブジェクトのWVPを更新 (speedMultiplier=0.0f
    // でアニメーションは進めない)
    ObjectBase::SetWvpIndex(1);
    EffectDefinition::SetWvpIndex(1);

    gameObjectManager_->UpdateAll(gameViewMat, 0.0f);
    stageSettings_->EditorUpdate(gameViewMat);
    effectManager_->EditorUpdate(gameViewMat);

    draw.SetCamera(gameCamera_.get());
    draw.SetEnvironmentTexture(skyBoxTexture_);
    gameObjectManager_->DrawAll(draw);
    stageSettings_->Draw(draw);
    effectManager_->Draw(draw);

    // Game View 内でも HUD (動的MSDFテキスト) を描画
    DrawHUD(draw);

    // SkyBoxの位置を元に戻す
    skyBox_->SetTransform(originalSkyBoxT);

    ObjectBase::SetWvpIndex(0);
    EffectDefinition::SetWvpIndex(0);
  });

  // スカイボックスの初期化
  skyBoxTexture_ = texture_.get()->CreateTexture("Resources/DDS/SnowWorld.dds");
  skyBox_.get()->Initialize(skyBoxTexture_);
  skyBox_.get()->SetShader("SkyBoxShader");
  skyBox_.get()->SetCullMode(kCullModeFront);
  skyBox_.get()->SetFrustumCullingEnabled(false);
  skyBox_.get()->SetLighting(false);
  skyBox_.get()->SetTransform(skyBoxTransform_);
  skyBox_.get()->name_ = "SkyBox";

  // プレイヤーの初期化
  ModelData modelData =
      AssimpLoadObjFile("Resources/Model/Player", "Player.obj");
  player_->Initialize(modelData);

  // ボスの初期化
  ModelData bossModelData = AssetManager::LoadModel(
      "Resources/Model/StylizedIceKing", "StylizedIceKing.obj");
  boss_->Initialize(bossModelData);
  boss_->SetName("Boss");

  // オブジェクトマネージャーへの登録
  gameObjectManager_->Clear();
  auto skyboxRenderObj = std::make_shared<RenderObject>(skyBox_);
  skyboxRenderObj->SetName("SkyBox");
  skyboxRenderObj->SetFrustumCullingEnabled(false);
  gameObjectManager_->AddObject(skyboxRenderObj);
  player_->SetName("Player");
  gameObjectManager_->AddObject(player_);
  gameObjectManager_->AddObject(boss_);

  // エディターでの保存・読み込み先をJsonSceneに設定
  EditorManager::SetSaveCallback([this](const std::string &filePath) {
    gameObjectManager_->SaveScene(filePath);
  });
  EditorManager::SetLoadCallback([this](const std::string &filePath) {
    gameObjectManager_->LoadScene(filePath);
  });
  EditorManager::SetFileDropCallback([this](const std::string &dropPath) {
    size_t lastSlash = dropPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
      std::string dirPath = dropPath.substr(0, lastSlash);
      std::string fileName = dropPath.substr(lastSlash + 1);

      size_t extPos = fileName.find_last_of(".");
      if (extPos != std::string::npos) {
        std::string ext = fileName.substr(extPos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".obj" || ext == ".gltf") {
          try {
            ModelData modelData = AssetManager::LoadModel(dirPath, fileName);
            auto model = std::make_shared<Model>();
            model->Initialize(modelData);
            model->name_ = fileName;

            auto renderObj = std::make_shared<RenderObject>(model);
            renderObj->SetName(fileName);
            gameObjectManager_->AddObject(renderObj);
          } catch (const std::exception &e) {
            // Error handling if loading fails
          }
        }
      }
    }
  });

  // ステージの初期化
  ModelData roadModelData =
      AssetManager::LoadModel("Resources/Block", "Block.obj");
  ModelData fallenTreeModel =
      AssetManager::LoadModel("Resources/Model/FallenTree", "FallenTree.obj");
  ModelData iceArchwayModel =
      AssetManager::LoadModel("Resources/Model/IceArchway", "IceArchway.obj");
  ModelData iceWallModel =
      AssetManager::LoadModel("Resources/Model/IceWall", "IceWall.obj");
  ModelData iceBomModel =
      AssetManager::LoadModel("Resources/Model/IceBom", "IceBom.obj");
  ModelData reflectingAttackModel =
      AssetManager::LoadModel("Resources/Model/ReflectingAttack",
                              "ReflectingAttack.obj");
  stageSettings_->Initialize(roadModelData, fallenTreeModel, iceArchwayModel,
                             iceWallModel, modelData, iceBomModel,
                             reflectingAttackModel, gameObjectManager_.get());

  // 指定したJsonファイルを初期シーンとして読み込む
  gameObjectManager_->LoadScene(initialSceneJson_);

  currentDistance_ = 0.0f;
  currentScore_ = 0.0f;
  bonusEnemyHitCount_ = 0;
  ChangePlayingState(PlayingState::ThreeLane, true);
}

void GameScene::Update() {
  ObjectBase::SetWvpIndex(0);
  EffectDefinition::SetWvpIndex(0);
  uiTimer_ += 1.0f / 60.0f;

#ifdef _DEBUG
  if (Input::PushKey(DIK_Q)) {
    ChangePlayingState(PlayingState::ThreeLane);
  }
  if (Input::PushKey(DIK_E)) {
    ChangePlayingState(PlayingState::OneLane);
  }
  if (Input::PushKey(DIK_F1)) {
    ColliderComponent::s_isDrawDebug_ = !ColliderComponent::s_isDrawDebug_;
  }
#endif // _DEBUG

  // PostEffect::SetActivePostEffect(PostEffect::Type::GaussianFilter);

  // Engine側のPlay/Stop状態に同期してゲームステートを切り替え
  bool isEnginePlaying = EditorManager::IsPlaying();
  if (isEnginePlaying && gameState_ == GameState::Editor) {
    ResetGame();
    gameState_ = GameState::Title;
    camera_->SetDebugCamera(false);
  } else if (!isEnginePlaying && gameState_ != GameState::Editor) {
    gameState_ = GameState::Editor;
    camera_->SetDebugCamera(true);
  }

  UpdateCameraTransition();

  camera_->Update();
  view = camera_->GetViewMatrix();

  gameCamera_->Update();

  // SkyBoxをカメラの位置に追従させる（無限遠の背景として機能させるため）
  Transform skyboxTransform = skyBox_->GetTransform();
  skyboxTransform.translate = camera_->GetTransform().translate;
  skyBox_->SetTransform(skyboxTransform);

  // skyBox_ is now updated in gameObjectManager_

  if (gameState_ == GameState::Title) {
    TitleUpdate();
  } else if (gameState_ == GameState::Playing) {
    PlayingUpdate();

    if (Input::PushKey(DIK_ESCAPE)) {
      gameState_ = GameState::Paused;
    }
  } else if (gameState_ == GameState::Paused) {
    PausedUpdate();
    if (Input::PushKey(DIK_ESCAPE)) {
      gameState_ = GameState::Playing;
    }
  } else if (gameState_ == GameState::PlayerHit) {
    PlayerHitUpdate();
  } else if (gameState_ == GameState::GameOver) {
    // 1でリスタート
    if (Input::PushKey(DIK_1)) {
      ResetGame();
      gameState_ = GameState::Playing;
    }
    // 2でタイトルへ
    if (Input::PushKey(DIK_2)) {
      ResetGame();
      gameState_ = GameState::Title;
    }
  } else if (gameState_ == GameState::Editor) {
    EditorUpdate();
  }

  // 雪などの常時出続けるパーティクルの更新
  effectManager_->AlwaysUpdate(view, camera_->GetTransform().translate);
}

void GameScene::Draw(class Draw &draw) {
  draw_ = &draw;
  // カメラの設定
  draw.SetCamera(camera_.get());
  // 背景の設定
  draw.SetEnvironmentTexture(skyBoxTexture_);

  // オブジェクトの一括描画（SkyBox, Player など）
  gameObjectManager_->DrawAll(draw);

  // ステージ描画（道路 + 障害物）
  stageSettings_->Draw(draw);

  // ポーズ中の描画
  if (gameState_ == GameState::Paused) {
    pauseSystem_->Draw(draw);
  }

  // ヒットエフェクトの描画
  effectManager_->Draw(draw);

  // HUDの描画
  DrawHUD(draw);
}

void GameScene::DrawHUD(class Draw &draw) {
  // 初回呼び出し時に頻出文字列をアトラスへ一括プリロード
  static bool s_preloaded = false;
  if (!s_preloaded && draw.GetTextRenderer() &&
      draw.GetTextRenderer()->GetAtlas()) {
    s_preloaded = true;
    draw.GetTextRenderer()->GetAtlas()->PreloadString(
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:/"
        ".mkmhpt%+-[]()!★◆▼▲●■░|"
        "一二三四五六七八九十百千万到達距離スコア速度最高記録ベストゲームオーバ"
        "ーリスタートリザルトへ戻る一時停止中現在獲得順位反撃チャンス左中央右打"
        "ち返せ跳ね返しボーナス敵撃破モードシールドバリアアクティブジャンプスラ"
        "イディング走るポーズキーもう一度遊ぶプレイ");
  }

  if (gameState_ == GameState::Title) {
    DrawTitleHUD(draw);
  } else if (gameState_ == GameState::GameOver) {
    DrawGameOverHUD(draw);
  } else if (gameState_ == GameState::Paused) {
    DrawPlayingHUD(draw);
    DrawPauseHUD(draw);
  } else if (gameState_ == GameState::PlayerHit) {
    DrawPlayingHUD(draw);
    // 衝突時の大迫力バナー
    draw.DrawFillRect(Vector2(400.0f, 240.0f), Vector2(480.0f, 100.0f),
                      Vector4(0.1f, 0.0f, 0.0f, 0.85f));
    draw.DrawFillRect(Vector2(400.0f, 240.0f), Vector2(480.0f, 3.0f),
                      Vector4(1.0f, 0.2f, 0.2f, 0.95f));
    draw.DrawMSDFString("★ CRASH! ★", Vector2(480.0f, 260.0f), 56.0f,
                        Vector4(1.0f, 0.25f, 0.25f, 1.0f), true,
                        Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.12f);
  } else if (gameState_ == GameState::Playing) {
    DrawPlayingHUD(draw);
    if (playingState_ == PlayingState::Boss && boss_->GetIsActive()) {
      DrawBossHUD(draw);
    }
    DrawControlsGuide(draw);
  } else if (gameState_ == GameState::Editor) {
    draw.DrawFillRect(Vector2(20.0f, 20.0f), Vector2(300.0f, 75.0f),
                      Vector4(0.04f, 0.06f, 0.1f, 0.8f));
    char distBuf[64];
    snprintf(distBuf, sizeof(distBuf), "[EDITOR] 距離: %.1f m",
             currentDistance_);
    draw.DrawMSDFString(distBuf, Vector2(30.0f, 28.0f), 22.0f,
                        Vector4(0.9f, 0.9f, 0.9f, 1.0f), true,
                        Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);
    char scoreBuf[64];
    snprintf(scoreBuf, sizeof(scoreBuf), "スコア: %.0f pt", currentScore_);
    draw.DrawMSDFString(scoreBuf, Vector2(30.0f, 58.0f), 20.0f,
                        Vector4(1.0f, 0.9f, 0.2f, 1.0f), true,
                        Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);
  }
}

void GameScene::DrawPlayingHUD(class Draw &draw) {
  // --- 1. 左上: メインステータスパネル（半透明背景カード付き） ---
  draw.DrawFillRect(Vector2(20.0f, 15.0f), Vector2(390.0f, 155.0f),
                    Vector4(0.04f, 0.07f, 0.12f, 0.82f));
  draw.DrawFillRect(Vector2(20.0f, 15.0f), Vector2(390.0f, 3.0f),
                    Vector4(0.2f, 0.6f, 0.9f, 0.9f));

  char distBuf[64];
  snprintf(distBuf, sizeof(distBuf), "距離: %.1f m", currentDistance_);
  draw.DrawMSDFString(distBuf, Vector2(32.0f, 24.0f), 32.0f,
                      Vector4(1.0f, 1.0f, 1.0f, 1.0f), true,
                      Vector4(0.05f, 0.15f, 0.25f, 1.0f), 0.08f);

  char scoreBuf[96];
  if (bonusEnemyHitCount_ > 0) {
    snprintf(scoreBuf, sizeof(scoreBuf), "スコア: %.0f pt  (+ボーナスx%d)",
             currentScore_, bonusEnemyHitCount_);
  } else {
    snprintf(scoreBuf, sizeof(scoreBuf), "スコア: %.0f pt", currentScore_);
  }
  draw.DrawMSDFString(scoreBuf, Vector2(32.0f, 66.0f), 26.0f,
                      Vector4(1.0f, 0.9f, 0.2f, 1.0f), true,
                      Vector4(0.15f, 0.10f, 0.0f, 1.0f), 0.08f);

  // スピードメーター（時速換算とプログレスゲージ）
  float currentSpeed = stageSettings_->GetScrollSpeed();
  float baseSpeed = stageSettings_->GetBaseScrollSpeed();
  float maxSpeed = stageSettings_->GetMaxScrollSpeed();
  float speedKm = currentSpeed * 300.0f;
  float speedRatio = (maxSpeed > baseSpeed)
                         ? ((currentSpeed - baseSpeed) / (maxSpeed - baseSpeed))
                         : 0.0f;
  if (speedRatio < 0.0f)
    speedRatio = 0.0f;
  if (speedRatio > 1.0f)
    speedRatio = 1.0f;

  std::string speedBar = "[";
  int totalSegments = 10;
  int filledSegments = static_cast<int>(speedRatio * totalSegments + 0.5f);
  for (int s = 0; s < totalSegments; s++) {
    if (s < filledSegments)
      speedBar += "■";
    else
      speedBar += "░";
  }
  speedBar += "]";

  char speedText[64];
  snprintf(speedText, sizeof(speedText), "速度: %.0f km/h  %s", speedKm,
           speedBar.c_str());
  Vector4 speedColor = Lerp(Vector4{0.3f, 0.9f, 1.0f, 1.0f},
                            Vector4{1.0f, 0.4f, 0.2f, 1.0f}, speedRatio);
  draw.DrawMSDFString(speedText, Vector2(32.0f, 102.0f), 20.0f, speedColor,
                      true, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  // ベスト記録（距離 / スコア）
  char bestText[96];
  snprintf(bestText, sizeof(bestText), "BEST: %.1f m  /  %.0f pt",
           topRankings_[0], topScoreRankings_[0]);
  draw.DrawMSDFString(bestText, Vector2(32.0f, 134.0f), 18.0f,
                      Vector4(0.8f, 0.85f, 0.9f, 0.85f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.06f);

  // --- 2. 右上: モード、バリア、プレイヤーステータスバッジ（半透明カード付き）
  // ---
  draw.DrawFillRect(Vector2(880.0f, 15.0f), Vector2(380.0f, 120.0f),
                    Vector4(0.04f, 0.07f, 0.12f, 0.82f));
  draw.DrawFillRect(Vector2(880.0f, 15.0f), Vector2(380.0f, 3.0f),
                    Vector4(0.2f, 0.6f, 0.9f, 0.9f));

  // モードバッジ
  if (playingState_ == PlayingState::ThreeLane) {
    draw.DrawMSDFString("[ 3-LANE RUN ]", Vector2(1040.0f, 25.0f), 24.0f,
                        Vector4(0.4f, 0.85f, 1.0f, 1.0f), true,
                        Vector4(0.0f, 0.15f, 0.35f, 1.0f), 0.08f);
  } else if (playingState_ == PlayingState::OneLane) {
    float remainDist = (200.0f - rightSideDistance_ > 0.0f)
                           ? (200.0f - rightSideDistance_)
                           : 0.0f;
    char oneLaneBuf[64];
    snprintf(oneLaneBuf, sizeof(oneLaneBuf), "[ 1-LANE DASH: 残り %.0f m ]",
             remainDist);
    draw.DrawMSDFString(oneLaneBuf, Vector2(910.0f, 25.0f), 24.0f,
                        Vector4(1.0f, 0.45f, 0.9f, 1.0f), true,
                        Vector4(0.35f, 0.0f, 0.35f, 1.0f), 0.08f);
  } else if (playingState_ == PlayingState::Boss) {
    float flashAlpha = 0.7f + 0.3f * std::sin(uiTimer_ * 8.0f);
    draw.DrawMSDFString("[ ! BOSS BATTLE ! ]", Vector2(1010.0f, 25.0f), 24.0f,
                        Vector4(1.0f, 0.25f, 0.25f, flashAlpha), true,
                        Vector4(0.4f, 0.0f, 0.0f, 1.0f), 0.08f);
  }

  // バリア（シールド）バッジ
  if (player_->GetHasBarrier()) {
    float pulse = 0.85f + 0.15f * std::sin(uiTimer_ * 6.0f);
    draw.DrawMSDFString("[◆ SHIELD: ACTIVE ]", Vector2(1005.0f, 62.0f), 22.0f,
                        Vector4(0.0f, 1.0f, 0.9f, pulse), true,
                        Vector4(0.0f, 0.35f, 0.35f, 1.0f), 0.08f);
  } else {
    draw.DrawMSDFString("[ SHIELD: OFF ]", Vector2(1060.0f, 62.0f), 22.0f,
                        Vector4(0.55f, 0.6f, 0.65f, 0.75f), true,
                        Vector4(0.1f, 0.1f, 0.1f, 1.0f), 0.06f);
  }

  // プレイヤーアクション状態バッジ
  if (player_->GetIsRolling()) {
    draw.DrawMSDFString("[▼ SLIDING ]", Vector2(1070.0f, 96.0f), 20.0f,
                        Vector4(1.0f, 0.75f, 0.2f, 1.0f), true,
                        Vector4(0.35f, 0.2f, 0.0f, 1.0f), 0.07f);
  } else if (player_->GetIsJumping()) {
    draw.DrawMSDFString("[▲ JUMPING ]", Vector2(1070.0f, 96.0f), 20.0f,
                        Vector4(0.3f, 0.85f, 1.0f, 1.0f), true,
                        Vector4(0.0f, 0.2f, 0.4f, 1.0f), 0.07f);
  } else {
    draw.DrawMSDFString("[● RUNNING ]", Vector2(1070.0f, 96.0f), 20.0f,
                        Vector4(0.35f, 0.95f, 0.45f, 1.0f), true,
                        Vector4(0.0f, 0.3f, 0.1f, 1.0f), 0.07f);
  }
}

void GameScene::DrawBossHUD(class Draw &draw) {
  // 中央上部: ボスパネル背景
  draw.DrawFillRect(Vector2(320.0f, 15.0f), Vector2(640.0f, 115.0f),
                    Vector4(0.10f, 0.03f, 0.03f, 0.85f));
  draw.DrawFillRect(Vector2(320.0f, 15.0f), Vector2(640.0f, 3.0f),
                    Vector4(0.9f, 0.2f, 0.2f, 0.9f));

  // ボスヘッダー
  draw.DrawMSDFString("=== BOSS: ICE KING ===", Vector2(440.0f, 24.0f), 28.0f,
                      Vector4(0.8f, 0.95f, 1.0f, 1.0f), true,
                      Vector4(0.0f, 0.2f, 0.5f, 1.0f), 0.08f);

  // ボスHPゲージ (最大20)
  int hp = boss_->GetHP();
  if (hp < 0)
    hp = 0;
  if (hp > 20)
    hp = 20;

  std::string hpGauge = "[";
  for (int i = 0; i < 20; i++) {
    if (i < hp)
      hpGauge += "■";
    else
      hpGauge += "░";
  }
  hpGauge += "]";

  char hpText[64];
  snprintf(hpText, sizeof(hpText), "HP %s %2d / 20", hpGauge.c_str(), hp);
  draw.DrawMSDFString(hpText, Vector2(395.0f, 58.0f), 22.0f,
                      Vector4(1.0f, 0.45f, 0.45f, 1.0f), true,
                      Vector4(0.2f, 0.0f, 0.0f, 1.0f), 0.07f);

  // 反撃（跳ね返し）基本操作ガイド
  draw.DrawMSDFString("[1] 左打ち返し  |  [2] 中央打ち返し  |  [3] 右打ち返し",
                      Vector2(400.0f, 92.0f), 20.0f,
                      Vector4(0.8f, 0.95f, 0.5f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  // 緑攻撃（跳ね返し可能弾）が反撃有効範囲（z: -15.0f
  // 〜 15.0f）にあるかチェック
  int reflectLane = -1;
  for (int j = 0; j < stageSettings_->GetMaxObstacles(); j++) {
    Obstacle *obs = stageSettings_->GetObstacle(j);
    if (!obs || !obs->GetIsActive() || obs->GetIsReflected())
      continue;

    if (obs->GetType() == Obstacle::Type::BossAttackReflectable) {
      float z = obs->GetTransform().translate.z;
      if (z > -15.0f && z < 15.0f) {
        float obsX = obs->GetTransform().translate.x;
        float laneW = stageSettings_->GetLaneWidth();
        int lane = 1;
        if (obsX < -laneW / 2.0f)
          lane = 0;
        else if (obsX > laneW / 2.0f)
          lane = 2;
        reflectLane = lane;
        break;
      }
    }
  }

  // 反撃チャンスのアラート点滅表示
  if (reflectLane != -1) {
    const char *keyName = (reflectLane == 0)   ? "1"
                          : (reflectLane == 1) ? "2"
                                               : "3";
    const char *laneName = (reflectLane == 0)   ? "左レーン"
                           : (reflectLane == 1) ? "中央レーン"
                                                : "右レーン";
    char alertBuf[96];
    snprintf(alertBuf, sizeof(alertBuf),
             ">>> 反撃チャンス！ [%s] キーで%s打ち返し！ <<<", keyName,
             laneName);

    float pulseScale = 0.8f + 0.2f * std::sin(uiTimer_ * 12.0f);
    draw.DrawFillRect(Vector2(290.0f, 125.0f), Vector2(700.0f, 40.0f),
                      Vector4(0.2f, 0.1f, 0.0f, 0.85f));
    draw.DrawMSDFString(alertBuf, Vector2(310.0f, 130.0f), 26.0f,
                        Vector4(1.0f, 0.95f, 0.15f, pulseScale), true,
                        Vector4(0.5f, 0.1f, 0.0f, 1.0f), 0.09f);
  }

  // ボス撃破時の演出バナー
  if (boss_->GetState() == BossState::Defeat) {
    draw.DrawFillRect(Vector2(360.0f, 170.0f), Vector2(560.0f, 95.0f),
                      Vector4(0.05f, 0.12f, 0.05f, 0.9f));
    draw.DrawFillRect(Vector2(360.0f, 170.0f), Vector2(560.0f, 3.0f),
                      Vector4(1.0f, 0.8f, 0.2f, 0.95f));
    draw.DrawMSDFString("★ BOSS DEFEATED! ★", Vector2(430.0f, 185.0f), 38.0f,
                        Vector4(1.0f, 0.9f, 0.2f, 1.0f), true,
                        Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.10f);
    draw.DrawMSDFString("撃破ボーナス獲得！", Vector2(510.0f, 230.0f), 24.0f,
                        Vector4(1.0f, 1.0f, 1.0f, 1.0f), true,
                        Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);
  }
}

void GameScene::DrawControlsGuide(class Draw &draw) {
  draw.DrawFillRect(Vector2(20.0f, 672.0f), Vector2(1240.0f, 36.0f),
                    Vector4(0.04f, 0.07f, 0.12f, 0.85f));
  draw.DrawFillRect(Vector2(20.0f, 672.0f), Vector2(1240.0f, 2.0f),
                    Vector4(0.3f, 0.5f, 0.7f, 0.8f));

  const char *guideText = "";
  if (playingState_ == PlayingState::ThreeLane) {
    guideText = "[A / D] レーン移動    [SPACE / W] ジャンプ    [S] スライド    "
                "[ESC] ポーズ";
  } else if (playingState_ == PlayingState::OneLane) {
    guideText = "[SPACE / W] ジャンプ    [S] スライド    [ESC] ポーズ  "
                "(※1レーン固定中)";
  } else if (playingState_ == PlayingState::Boss) {
    guideText = "[A / D] 移動    [1 / 2 / 3] レーン別反撃    [SPACE / W] "
                "ジャンプ    [ESC] ポーズ";
  }

  draw.DrawMSDFString(guideText, Vector2(30.0f, 680.0f), 18.0f,
                      Vector4(0.85f, 0.9f, 0.95f, 0.9f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.06f);
}

void GameScene::DrawPauseHUD(class Draw &draw) {
  // 全画面暗転オーバーレイ
  draw.DrawFillRect(Vector2(0.0f, 0.0f), Vector2(1280.0f, 720.0f),
                    Vector4(0.0f, 0.0f, 0.0f, 0.65f));

  // 中央モーダルカード
  draw.DrawFillRect(Vector2(360.0f, 160.0f), Vector2(560.0f, 410.0f),
                    Vector4(0.06f, 0.08f, 0.12f, 0.94f));
  draw.DrawFillRect(Vector2(360.0f, 160.0f), Vector2(560.0f, 4.0f),
                    Vector4(1.0f, 0.8f, 0.2f, 0.95f));

  draw.DrawMSDFString("=== PAUSE ===", Vector2(490.0f, 190.0f), 52.0f,
                      Vector4(1.0f, 0.9f, 0.2f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.10f);

  draw.DrawMSDFString("ゲーム一時停止中", Vector2(540.0f, 255.0f), 22.0f,
                      Vector4(0.85f, 0.85f, 0.85f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  draw.DrawFillRect(Vector2(400.0f, 290.0f), Vector2(480.0f, 2.0f),
                    Vector4(0.3f, 0.4f, 0.5f, 0.7f));

  char pDistBuf[64];
  snprintf(pDistBuf, sizeof(pDistBuf), "現在の到達距離:  %.1f m",
           currentDistance_);
  draw.DrawMSDFString(pDistBuf, Vector2(430.0f, 315.0f), 26.0f,
                      Vector4(1.0f, 1.0f, 1.0f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  char pScoreBuf[64];
  snprintf(pScoreBuf, sizeof(pScoreBuf), "現在のスコア:    %.0f pt",
           currentScore_);
  draw.DrawMSDFString(pScoreBuf, Vector2(430.0f, 355.0f), 26.0f,
                      Vector4(1.0f, 0.9f, 0.2f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  draw.DrawFillRect(Vector2(400.0f, 400.0f), Vector2(480.0f, 2.0f),
                    Vector4(0.3f, 0.4f, 0.5f, 0.7f));

  draw.DrawMSDFString("[ ESC ] ゲームを再開 (RESUME)", Vector2(430.0f, 425.0f),
                      24.0f, Vector4(0.3f, 0.9f, 1.0f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  draw.DrawMSDFString("[  1  ] 最初からリスタート (RESTART)",
                      Vector2(430.0f, 465.0f), 24.0f,
                      Vector4(0.9f, 0.9f, 0.9f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  draw.DrawMSDFString("[  2  ] タイトルへ戻る (TITLE)", Vector2(430.0f, 505.0f),
                      24.0f, Vector4(0.9f, 0.9f, 0.9f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);
}

void GameScene::DrawGameOverHUD(class Draw &draw) {
  // 全画面暗転オーバーレイ
  draw.DrawFillRect(Vector2(0.0f, 0.0f), Vector2(1280.0f, 720.0f),
                    Vector4(0.0f, 0.0f, 0.0f, 0.72f));

  // 中央モーダルカード
  draw.DrawFillRect(Vector2(200.0f, 60.0f), Vector2(880.0f, 570.0f),
                    Vector4(0.06f, 0.08f, 0.12f, 0.95f));
  draw.DrawFillRect(Vector2(200.0f, 60.0f), Vector2(880.0f, 4.0f),
                    Vector4(0.9f, 0.2f, 0.2f, 0.95f));

  draw.DrawMSDFString("GAME OVER", Vector2(480.0f, 85.0f), 56.0f,
                      Vector4(1.0f, 0.25f, 0.25f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.10f);

  bool isNewDistRecord =
      (topRankings_[0] > 0.0f && currentDistance_ >= topRankings_[0]);
  bool isNewScoreRecord =
      (topScoreRankings_[0] > 0.0f && currentScore_ >= topScoreRankings_[0]);

  char distBuf[96];
  snprintf(distBuf, sizeof(distBuf), "到達距離: %.1f m  %s", currentDistance_,
           isNewDistRecord ? "★ NEW RECORD! ★" : "");
  draw.DrawMSDFString(distBuf, Vector2(260.0f, 160.0f), 26.0f,
                      isNewDistRecord ? Vector4(1.0f, 0.9f, 0.2f, 1.0f)
                                      : Vector4(1.0f, 1.0f, 1.0f, 1.0f),
                      true, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  char scoreBuf[96];
  snprintf(scoreBuf, sizeof(scoreBuf), "最終スコア: %.0f pt  %s", currentScore_,
           isNewScoreRecord ? "★ NEW RECORD! ★" : "");
  draw.DrawMSDFString(scoreBuf, Vector2(260.0f, 200.0f), 26.0f,
                      Vector4(1.0f, 0.9f, 0.2f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);

  char bonusBuf[64];
  snprintf(bonusBuf, sizeof(bonusBuf), "ボーナス敵撃破: %d 体",
           bonusEnemyHitCount_);
  draw.DrawMSDFString(bonusBuf, Vector2(260.0f, 240.0f), 22.0f,
                      Vector4(0.6f, 0.9f, 1.0f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.06f);

  // 区切りライン
  draw.DrawFillRect(Vector2(240.0f, 280.0f), Vector2(800.0f, 2.0f),
                    Vector4(0.3f, 0.4f, 0.5f, 0.7f));

  // ランキング 2カラム表示
  // 左カラム: 距離ランキング TOP 3
  draw.DrawMSDFString("【 距離ランキング TOP 3 】", Vector2(260.0f, 305.0f),
                      22.0f, Vector4(0.4f, 0.85f, 1.0f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);
  for (int i = 0; i < 3; i++) {
    char rBuf[64];
    if (topRankings_[i] > 0.0f) {
      snprintf(rBuf, sizeof(rBuf), " %d位: %.1f m", i + 1, topRankings_[i]);
    } else {
      snprintf(rBuf, sizeof(rBuf), " %d位: ---", i + 1);
    }
    Vector4 rankColor = (i == 0)   ? Vector4(1.0f, 0.9f, 0.2f, 1.0f)
                        : (i == 1) ? Vector4(0.85f, 0.85f, 0.9f, 1.0f)
                                   : Vector4(0.85f, 0.65f, 0.45f, 1.0f);
    draw.DrawMSDFString(rBuf, Vector2(270.0f, 345.0f + i * 35.0f), 20.0f,
                        rankColor, true, Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                        0.06f);
  }

  // 右カラム: スコアランキング TOP 3
  draw.DrawMSDFString("【 スコアランキング TOP 3 】", Vector2(660.0f, 305.0f),
                      22.0f, Vector4(1.0f, 0.85f, 0.3f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);
  for (int i = 0; i < 3; i++) {
    char rBuf[64];
    if (topScoreRankings_[i] > 0.0f) {
      snprintf(rBuf, sizeof(rBuf), " %d位: %.0f pt", i + 1,
               topScoreRankings_[i]);
    } else {
      snprintf(rBuf, sizeof(rBuf), " %d位: ---", i + 1);
    }
    Vector4 rankColor = (i == 0)   ? Vector4(1.0f, 0.9f, 0.2f, 1.0f)
                        : (i == 1) ? Vector4(0.85f, 0.85f, 0.9f, 1.0f)
                                   : Vector4(0.85f, 0.65f, 0.45f, 1.0f);
    draw.DrawMSDFString(rBuf, Vector2(670.0f, 345.0f + i * 35.0f), 20.0f,
                        rankColor, true, Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                        0.06f);
  }

  // 区切りライン
  draw.DrawFillRect(Vector2(240.0f, 470.0f), Vector2(800.0f, 2.0f),
                    Vector4(0.3f, 0.4f, 0.5f, 0.7f));

  draw.DrawMSDFString("[ 1 キー ] もう一度プレイ (RESTART)   |   [ 2 キー ] "
                      "タイトルへ戻る (TITLE)",
                      Vector2(260.0f, 505.0f), 22.0f,
                      Vector4(0.9f, 0.95f, 1.0f, 1.0f), true,
                      Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.07f);
}

void GameScene::DrawTitleHUD(class Draw &draw) {
  // 背景のタイトル画像を描画
  if (titleSprite_) {
    titleSprite_->Update(titleSpriteData_);
    draw.DrawSprite(titleSprite_.get());
  }

  // スタート案内テキストの点滅表示
  float blink = sinf(uiTimer_ * 5.0f);
  if (blink > -0.2f) {
    draw.DrawMSDFString("PRESS SPACE OR [A] BUTTON TO START",
                        Vector2(370.0f, 600.0f), 28.0f,
                        Vector4(1.0f, 1.0f, 1.0f, 1.0f), true,
                        Vector4(0.0f, 0.0f, 0.0f, 1.0f), 0.12f);
  }
}

void GameScene::PlayerHitUpdate() {
  // カメラやビューの更新は GameScene::Update で行われている
  effectManager_->PlayerHitUpdate(view);

  // プレイヤーのノックバックアニメーションを更新
  // (GameScene側の全体更新は停止し、プレイヤーのみ更新)
  player_->Update(view, 1.0f);

  if (player_->IsHitAnimationFinished()) {
    gameState_ = GameState::GameOver;
  }
}

void GameScene::PlayingUpdate() {
  float timeScale = EditorManager::GetPlaySpeed();
  float speedMultiplier = 1.0f;
  if (stageSettings_->GetBaseScrollSpeed() > 0.0f) {
    speedMultiplier =
        stageSettings_->GetScrollSpeed() / stageSettings_->GetBaseScrollSpeed();
  }

  currentDistance_ += stageSettings_->GetScrollSpeed() * timeScale;
  currentScore_ = currentDistance_ + (bonusEnemyHitCount_ * 200.0f *
                                      stageSettings_->GetScrollSpeed());

  CheckKeepRolling();

  // 右サイドモード（カメラアイテム取得後）の更新
  if (isRightSideMode_) {
    rightSideDistance_ += stageSettings_->GetScrollSpeed() * timeScale;
    if (rightSideDistance_ >= 200.0f) {
      ChangePlayingState(PlayingState::ThreeLane);
    }
  }

  // ボス戦の更新
  if (playingState_ == PlayingState::Boss) {
    if (!boss_->GetIsActive() && !isCameraTransitioning_ &&
        !isCameraTransitionPending_) {
      ChangePlayingState(PlayingState::ThreeLane);
    } else if (boss_->GetIsActive() && boss_->GetState() == BossState::Battle) {
      bossAttackTimer_ += timeScale;
      // 攻撃の生成（約2秒に1回）
      if (bossAttackTimer_ >= 120.0f) {
        bossAttackTimer_ -= 120.0f;

        // 3レーンのうち、1つを安全地帯、1つを白、1つを緑にする
        int safeLane = rand() % 3;
        int greenLane = (safeLane + 1 + rand() % 2) % 3;

        for (int i = 0; i < 3; i++) {
          if (i == safeLane)
            continue;

          Obstacle::Type type = Obstacle::Type::BossAttack;
          if (i == greenLane) {
            type = Obstacle::Type::BossAttackReflectable;
          }

          // 未使用のObstacleを探す
          for (int j = 0; j < stageSettings_->GetMaxObstacles(); j++) {
            Obstacle *obs = stageSettings_->GetObstacle(j);
            if (!obs->GetIsActive()) {
              obs->SetType(type);
              obs->SetDropHeight(bossAttackDropHeight_);
              obs->SetFallDuration(bossAttackFallDuration_);
              float x = (i - 1) * stageSettings_->GetLaneWidth();
              obs->Spawn(x, 2.0f + obs->GetCollisionHeight() * 0.5f,
                         bossAttackSpawnZ_); // ボスの攻撃を奥の上空から降らせて出現させる
              break;
            }
          }
        }
      }

      // 跳ね返し入力判定
      bool push1 = Input::PushKey(DIK_1) || Input::PushKey(DIK_NUMPAD1);
      bool push2 = Input::PushKey(DIK_2) || Input::PushKey(DIK_NUMPAD2);
      bool push3 = Input::PushKey(DIK_3) || Input::PushKey(DIK_NUMPAD3);

      AABB bossAABB =
          Collision::MakeAABB(boss_->GetTransform(), 5.0f, 5.0f, 5.0f);

      for (int j = 0; j < stageSettings_->GetMaxObstacles(); j++) {
        Obstacle *obs = stageSettings_->GetObstacle(j);
        if (!obs->GetIsActive())
          continue;

        // 着地時の砂煙・衝撃波エフェクト
        if (obs->GetJustLanded()) {
          effectManager_->EmitDust(obs->GetTransform().translate);
          effectManager_->EmitShockwave(obs->GetTransform().translate);
        }

        if (obs->GetType() == Obstacle::Type::BossAttackReflectable &&
            !obs->GetIsReflected() && !obs->GetIsFalling()) {
          float obsX = obs->GetTransform().translate.x;
          float laneW = stageSettings_->GetLaneWidth();
          int lane = 1; // 0:Left, 1:Center, 2:Right
          if (obsX < -laneW / 2.0f)
            lane = 0;
          else if (obsX > laneW / 2.0f)
            lane = 2;

          // プレイヤーの手前にいる時に跳ね返せる
          float z = obs->GetTransform().translate.z;
          float reflectMinZ = (std::min)(-15.0f, bossAttackSpawnZ_ - 1.0f);
          if (z > reflectMinZ && z < 15.0f) {
            if ((lane == 0 && push1) || (lane == 1 && push2) ||
                (lane == 2 && push3)) {
              obs->SetReflected(true);
              obs->SetReflectedTarget(boss_->GetTransform().translate);
              effectManager_->EmitShockwave(obs->GetTransform().translate);
            }
          }
        } else if (obs->GetIsReflected()) {
          // 跳ね返った障害物とボスとの当たり判定
          AABB obsAABB = Collision::MakeAABB(
              obs->GetTransform(), obs->GetCollisionWidth(),
              obs->GetCollisionHeight(), obs->GetCollisionDepth());
          if (Collision::CheckAABB(bossAABB, obsAABB)) {
            obs->Deactivate(); // 障害物を消す
            boss_->OnDamage();
            effectManager_->EmitHitEffect(boss_->GetTransform().translate);

            if (boss_->GetState() == BossState::Defeat) {
              // 画面内のボス攻撃をすべて消す
              for (int k = 0; k < stageSettings_->GetMaxObstacles(); k++) {
                Obstacle *o = stageSettings_->GetObstacle(k);
                if (o->GetIsActive() &&
                    (o->GetType() == Obstacle::Type::BossAttack ||
                     o->GetType() == Obstacle::Type::BossAttackReflectable)) {
                  o->OnBlowAway();
                }
              }
            }
          }
        }
      }
    }
  }

  // Update Player lane constraints
  player_->SetLaneLimits(stageSettings_->GetMinLaneIndex(),
                         stageSettings_->GetMaxLaneIndex(),
                         stageSettings_->GetLaneWidth());

  // オブジェクトの一括更新
  gameObjectManager_->UpdateAll(view, speedMultiplier * timeScale);

  stageSettings_->Update(view, timeScale);

  // 以前の当たり判定チェック
  CheckCollisions();

  // Componentベースの当たり判定チェック
  collisionManager_->UpdateCollisions(gameObjectManager_.get());

  effectManager_->PlayingUpdate(view, player_->GetTransform().translate);

  // 走っている間（転がっていなくて地面にいる時）	//
  // プレイヤーの足元に砂埃エフェクトを生成
  if (!player_->GetIsRolling() &&
      player_->GetTransform().translate.y <= 3.01f) {
    effectManager_->EmitDust(player_->GetTransform().translate);
  }
}

void GameScene::TitleUpdate() {
  // 背景シーンの更新（スクロールなし、オブジェクト描画準備）
  gameObjectManager_->UpdateAll(view, 0.0f);
  stageSettings_->EditorUpdate(view);

  // スペースキーまたはゲームパッドAボタンでゲーム開始
  if (Input::PushKey(DIK_SPACE) ||
      GamePadInput::PressButton(XINPUT_GAMEPAD_A)) {
    ResetGame();
    gameState_ = GameState::Playing;
  }
}

void GameScene::PausedUpdate() {
  pauseSystem_->Update();
  if (Input::PushKey(DIK_1)) {
    ResetGame();
    gameState_ = GameState::Playing;
  }
  if (Input::PushKey(DIK_2)) {
    ResetGame();
    gameState_ = GameState::Title;
  }
}

void GameScene::EditorUpdate() {
  // Editor mode doesn't progress the game scroll or obstacle positions.
  // But we still want to update objects (like their transforms).
  gameObjectManager_->UpdateAll(view, 0.0f);
  stageSettings_->EditorUpdate(view);

  // パーティクルがデバッグカメラに対応するように、EditorUpdate() を呼び出す
  effectManager_->EditorUpdate(view);
}

void GameScene::CheckCollisions() {
  // プレイヤーのAABBを生成
  const Transform &playerTransform = player_->GetTransform();
  float playerHeight =
      player_->GetIsRolling() ? 0.5f : 1.5f; // 転がり中は低くなる
  AABB playerAABB =
      Collision::MakeAABB(playerTransform, 0.8f, playerHeight, 0.8f);

  // 全障害物との当たり判定
  for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
    Obstacle *obstacle = stageSettings_->GetObstacle(i);
    if (!obstacle->GetIsActive() || obstacle->GetIsHit())
      continue;

    AABB obstacleAABB = Collision::MakeAABB(
        obstacle->GetTransform(), obstacle->GetCollisionWidth(),
        obstacle->GetCollisionHeight(), obstacle->GetCollisionDepth());

    if (Collision::CheckAABB(playerAABB, obstacleAABB)) {
      // 誘導床（GuideFloor）の判定
      if (obstacle->GetType() == Obstacle::Type::GuideFloor) {
        // プレイヤーを滑らかに中央へ誘導 (30フレーム)
        player_->StartForceToCenter(30.0f);

        // 【演出ポイント: パーティクル】
        // 加速や誘導を示すスピード線のエフェクトや、足元の衝撃波を出す
        // effectManager_->EmitGuideEffect(player_->GetTransform().translate);

        // 【演出ポイント: サウンド】
        // SoundManager::Play("GuideDash_SE"); // シューッというSE等

        continue; // ゲームオーバーにはならない
      }

      if (obstacle->GetType() == Obstacle::Type::Bonus) {
        // ボーナスエネミーに当たった場合の処理（吹き飛ばす）
        obstacle->OnBlowAway();
        bonusEnemyHitCount_++; // スコア（距離）ボーナス

        // プレイヤーの足元にRingエフェクトを出す
        effectManager_->EmitShockwave(player_->GetTransform().translate);

        continue; // ゲームオーバーにはならず、次の判定へ
      }

      if (obstacle->GetType() == Obstacle::Type::CameraItem) {
        obstacle->OnHit();
        ChangePlayingState(PlayingState::OneLane);

        // プレイヤーの足元にRingエフェクトを出す(ボーナスと同様の演出)
        effectManager_->EmitShockwave(player_->GetTransform().translate);

        continue; // ゲームオーバーにはならず、次の判定へ
      }

      if (obstacle->GetType() == Obstacle::Type::BarrierItem) {
        obstacle->OnHit();
        player_->SetHasBarrier(true);
        effectManager_->EmitBarrier(player_->GetTransform().translate);
        effectManager_->EmitShockwave(player_->GetTransform().translate);
        continue;
      }

      if (obstacle->GetType() == Obstacle::Type::ClearItem) {
        obstacle->OnHit();
        effectManager_->EmitShockwave(player_->GetTransform().translate);

        // 画面内の障害物を吹き飛ばす
        for (int j = 0; j < stageSettings_->GetMaxObstacles(); j++) {
          Obstacle *obs = stageSettings_->GetObstacle(j);
          if (obs->GetIsActive() && (obs->GetType() == Obstacle::Type::Low ||
                                     obs->GetType() == Obstacle::Type::High ||
                                     obs->GetType() == Obstacle::Type::Wall)) {
            obs->OnBlowAway();
          }
        }
        continue;
      }

      if (obstacle->GetType() == Obstacle::Type::BossItem) {
        obstacle->OnHit();
        ChangePlayingState(PlayingState::Boss);
        effectManager_->EmitShockwave(player_->GetTransform().translate);
        continue;
      }

      if (obstacle->GetType() == Obstacle::Type::BossAttack ||
          obstacle->GetType() == Obstacle::Type::BossAttackReflectable) {
        // 跳ね返されている緑障害物はプレイヤーに当たらない
        if (obstacle->GetIsReflected())
          continue;
        // 落下中（上空にいる間）はプレイヤーに当たらない
        if (obstacle->GetIsFalling())
          continue;
      }

      // プレイヤーがバリアを持っている場合は消費して防ぐ
      if (player_->GetHasBarrier()) {
        player_->SetHasBarrier(false);
        obstacle->OnBlowAway();
        effectManager_->EmitHitEffect(player_->GetTransform().translate);
        effectManager_->BreakBarrier(player_->GetTransform().translate);
        continue; // ゲームオーバーにならず次へ
      }

      // 衝突！ヒット演出へ移行
      gameState_ = GameState::PlayerHit;
      if (playingState_ == PlayingState::Boss && boss_->GetIsActive()) {
        boss_->ChangeState(BossState::Victory);
      }

      stageSettings_->SetGameOver(true);
      // PostEffect::SetActivePostEffect(PostEffect::Type::GrayScale);

      // プレイヤーのヒットアニメーション開始（Low障害物なら前へ転がる）
      bool isTrip = (obstacle->GetType() == Obstacle::Type::Low);
      player_->OnHit(isTrip);

      // エフェクトの発生位置をプレイヤーから取得する
      effectManager_->EmitHitEffect(player_->GetTransform().translate);

      // ランキング更新
      UpdateRanking();
      UpdateScoreRanking();

      break;
    }
  }
}

void GameScene::UpdateRanking() {
  // 降順ソートでトップ3を保持
  for (int i = 0; i < 3; i++) {
    if (currentDistance_ > topRankings_[i]) {
      // シフト
      for (int j = 2; j > i; j--) {
        topRankings_[j] = topRankings_[j - 1];
      }
      topRankings_[i] = currentDistance_;
      break;
    }
  }
}

void GameScene::UpdateScoreRanking() {
  // 降順ソートでトップ3を保持
  for (int i = 0; i < 3; i++) {
    if (currentScore_ > topScoreRankings_[i]) {
      // シフト
      for (int j = 2; j > i; j--) {
        topScoreRankings_[j] = topScoreRankings_[j - 1];
      }
      topScoreRankings_[i] = currentScore_;
      break;
    }
  }
}

void GameScene::CheckKeepRolling() {
  bool keepRolling = false;
  if (player_->GetIsRolling()) {
    // Calculate a "standing up" AABB for the player
    const Transform &playerTransform = player_->GetTransform();
    AABB standingAABB = Collision::MakeAABB(playerTransform, 0.8f, 1.5f, 0.8f);

    for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
      Obstacle *obstacle = stageSettings_->GetObstacle(i);
      if (!obstacle->GetIsActive() || obstacle->GetIsHit())
        continue;

      if (obstacle->GetType() == Obstacle::Type::High) {
        AABB obstacleAABB = Collision::MakeAABB(
            obstacle->GetTransform(), obstacle->GetCollisionWidth(),
            obstacle->GetCollisionHeight(), obstacle->GetCollisionDepth());

        // もし立ち上がったら当たる位置にいるか？
        if (Collision::CheckAABB(standingAABB, obstacleAABB)) {
          // プレイヤーの中心が障害物の中心より奥（Z座標が大きい）なら
          if (playerTransform.translate.z >
              obstacle->GetTransform().translate.z) {
            keepRolling = true;
            break;
          }
        }
      }
    }
  }
  player_->SetKeepRolling(keepRolling);
}

void GameScene::ChangePlayingState(PlayingState newState, bool force) {
  if (!force && playingState_ == newState)
    return;

  playingState_ = newState;

  Transform target;
  target.scale = {1.0f, 1.0f, 1.0f};
  int laneCount = 3;

  switch (playingState_) {
  case PlayingState::ThreeLane:
    target.rotate = {0.3f, 0.0f, 0.0f};
    target.translate = {0.0f, 8.0f, -15.0f};
    laneCount = 3;
    isRightSideMode_ = false;
    rightSideDistance_ = 0.0f;
    break;

  case PlayingState::OneLane:
    target.rotate = {0.3f, -1.0472f, 0.0f};
    target.translate = {30.0f, 15.0f, -5.0f};
    laneCount = 1;
    isRightSideMode_ = true;
    rightSideDistance_ = 0.0f;
    break;

  case PlayingState::Boss:
    // ボス用のカメラ位置（ここをいじればすぐに変更できます）
    target.rotate = {0.3f, 3.25f, 0.0f};
    target.translate = {0.0f, 8.0f, 14.0f};
    laneCount = 3;
    isRightSideMode_ = false;
    rightSideDistance_ = 0.0f;
    stageSettings_->SetSpawningPaused(true);
    bossAttackTimer_ = 0.0f;
    break;
  }

  StartCameraTransition(target, laneCount);
}

void GameScene::StartCameraTransition(const Transform &targetTransform,
                                      int laneCount) {
  // 障害物の生成を即座に停止
  stageSettings_->SetSpawningPaused(true);

  // トランジションの予約を行う
  isCameraTransitionPending_ = true;
  pendingCameraTargetTransform_ = targetTransform;
  pendingLaneCount_ = laneCount;
}

void GameScene::UpdateCameraTransition() {
  if (isCameraTransitionPending_) {
    // アクティブな障害物が残っているかチェック
    bool hasActiveObstacles = false;
    for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
      Obstacle *obstacle = stageSettings_->GetObstacle(i);
      if (obstacle->GetIsActive()) {
        hasActiveObstacles = true;
        break;
      }
    }

    // 障害物が全て消えたら、実際のトランジションを開始する
    if (!hasActiveObstacles) {
      isCameraTransitionPending_ = false;
      isCameraTransitioning_ = true;
      cameraTransitionTimer_ = 0.0f;
      startCameraTransform_ = cameraTransform_;
      targetCameraTransform_ = pendingCameraTargetTransform_;
      stageSettings_->SetLaneCount(pendingLaneCount_);
    }
  }

  if (!isCameraTransitioning_)
    return;

  cameraTransitionTimer_ +=
      1.0f / 60.0f; // 毎フレームの時間を加算 (FPS固定なら)
  float t = cameraTransitionTimer_ / cameraTransitionDuration_;

  if (t >= 1.0f) {
    t = 1.0f;
    isCameraTransitioning_ = false;
    if (playingState_ != PlayingState::Boss) {
      stageSettings_->SetSpawningPaused(false);
    } else {
      // カメラ遷移が終わってからボスを出現させる
      if (!boss_->GetIsActive()) {
        boss_->Spawn(-6.0f, 4.0f, -2.0f);
      }
    }
  }

  // easeInOut（スムーズな動きのため）
  float easeT = t * t * (3.0f - 2.0f * t);

  cameraTransform_.translate = Lerp(startCameraTransform_.translate,
                                    targetCameraTransform_.translate, easeT);

  // 弧を描くためのオフセット (Y軸方向に膨らむ)
  float arcHeight = 10.0f;
  cameraTransform_.translate.y += std::sin(easeT * 3.14159265f) * arcHeight;

  cameraTransform_.rotate =
      Lerp(startCameraTransform_.rotate, targetCameraTransform_.rotate, easeT);
  cameraTransform_.scale =
      Lerp(startCameraTransform_.scale, targetCameraTransform_.scale, easeT);

  camera_->SetTransform(cameraTransform_);
  gameCamera_->SetTransform(cameraTransform_);
}
