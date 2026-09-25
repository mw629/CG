#include "StageSettings.h"
#include "AssetManager.h"
#include "PSO/PipelineState.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>

void StageSettings::Initialize(ModelData roadModelData,
                               ModelData obstacleModelData,
                               ModelData bonusModelData,
                               class GameObjectManager *manager) {
  // 乱数の初期化
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  // 初回生成間隔の計算
  CalculateNextObstacleInterval();

  // グラウンドテクスチャをロード
  texture_->CreateTexture("Resources/Model/Ground/Ground.png");
  texture_->CreateTexture("Resources/Texture/white64x64.png");

  planeModelData_ =
      AssetManager::LoadModel("Resources/Model/obj", "ocean_plane.obj");

  roadModelData_ = roadModelData;
  manager_ = manager;

  // 道路チャンクの初期化
  GenerateRoadChunks();

  // 障害物の初期化
  for (int i = 0; i < kMaxObstacles_; i++) {
    obstacles_[i] = std::make_shared<Obstacle>();
    obstacles_[i]->SetName("Obstacle " + std::to_string(i));

    // ランダムなタイプで初期化
    Obstacle::Type type = static_cast<Obstacle::Type>(std::rand() % 3);
    obstacles_[i]->Initialize(obstacleModelData, bonusModelData, type);

    if (manager)
      manager->AddObject(obstacles_[i]);
  }

  // アイテムクールタイム設定の初期化（秒単位）
  itemCoolDowns_[Obstacle::Type::Bonus] = {5.0f, 0.0f};        // ボーナス: 5秒
  itemCoolDowns_[Obstacle::Type::BarrierItem] = {15.0f, 0.0f}; // バリア: 15秒
  itemCoolDowns_[Obstacle::Type::ClearItem] = {20.0f, 0.0f};   // 全消去: 20秒
  itemCoolDowns_[Obstacle::Type::CameraItem] = {
      30.0f, 10.0f}; // カメラ: 30秒（開始時10秒猶予）
  itemCoolDowns_[Obstacle::Type::BossItem] = {
      45.0f, 20.0f}; // ボス: 45秒（開始時20秒猶予）
}

void StageSettings::CalculateNextObstacleInterval() {
  // 猶予フレームを最小〜最大の間でランダムに選定（等間隔にならないようにバリエーションを持たせる）
  float graceFrames = minGraceFrames_; // 決定された猶予フレーム数
  int frameRange =
      static_cast<int>(maxGraceFrames_ - minGraceFrames_); // 変動フレーム幅
  if (frameRange > 0) {
    graceFrames += static_cast<float>(std::rand() % (frameRange + 1));
  }

  // 移動速度（スクロール速度）に（アクション所要フレーム＋猶予フレーム）を乗算して次回間隔（距離）を計算
  float effectiveSpeed =
      (std::max)(scrollSpeed_,
                 0.05f); // 停止時や低速時の0除算・0距離を防ぐ実効速度
  float totalFrames =
      baseActionFrames_ + graceFrames; // 回避に必要な合計フレーム数
  float calculatedDistance =
      effectiveSpeed * totalFrames; // 移動速度と猶予フレームから計算された距離

  // 確実に避けられる距離範囲（最小距離〜最大距離）に制限
  // 速度が高速になってもアクション完了＋最小猶予フレーム（5f）を下回らないよう上限を動的ガード
  float minRequiredDistance =
      effectiveSpeed * (baseActionFrames_ + minGraceFrames_);
  float dynamicMaxDistance =
      (std::max)(maxObstacleDistance_, minRequiredDistance);

  obstacleInterval_ =
      (std::min)((std::max)(minObstacleDistance_, calculatedDistance),
                 dynamicMaxDistance); // 次回生成までの距離
}

void StageSettings::GenerateRoadChunks(Matrix4x4 view) {
  // 既存のZ座標を保存（ゲーム中にレーン数が変わった際に地面が飛ぶのを防ぐため）
  std::vector<float> currentZs(kChunkCount_);
  for (int zIndex = 0; zIndex < kChunkCount_; zIndex++) {
    if (zIndex < roadTransforms_.size() && !roadTransforms_[zIndex].empty()) {
      currentZs[zIndex] = roadTransforms_[zIndex][0].translate.z;
    } else {
      currentZs[zIndex] = static_cast<float>(zIndex) * chunkLength_;
    }
  }

  // 既存のチャンクをマネージャーから削除し、プールに保存する
  for (auto &row : roadChunks_) {
    for (auto &chunk : row) {
      if (manager_)
        manager_->RemoveObject(chunk);
      chunkPool_.push_back(chunk);
    }
  }
  roadChunks_.clear();
  roadTransforms_.clear();

  roadChunks_.resize(kChunkCount_);
  roadTransforms_.resize(kChunkCount_);

  // 現在のレーン幅・数にあわせてチャンクを生成
  for (int zIndex = 0; zIndex < kChunkCount_; zIndex++) {
    roadChunks_[zIndex].resize(laneCount_);
    roadTransforms_[zIndex].resize(laneCount_);

    for (int laneIdx = 0; laneIdx < laneCount_; laneIdx++) {
      std::shared_ptr<RenderObject> renderObj;

      // プールから取得するか、新規作成する
      if (!chunkPool_.empty()) {
        renderObj = chunkPool_.back();
        chunkPool_.pop_back();
      } else {
        auto roadModel = std::make_shared<Model>();
        roadModel->Initialize(roadModelData_);
        if (auto matComp = roadModel->GetComponent<MaterialComponent>()) {
          matComp->SetTexturePath("Resources/Model/Ground/Ground.png");
        }
        roadModel->SetLighting(false);
        roadModel->SetTexture(
            texture_->TextureData("Resources/Model/Ground/Ground.png"));

        renderObj = std::make_shared<RenderObject>(roadModel);
      }

      renderObj->SetName("RoadChunk_" + std::to_string(zIndex) + "_Lane_" +
                         std::to_string(laneIdx));

      float effectiveLaneWidth = GetEffectiveLaneWidth();
      int logicalLane = minLaneIndex_ + laneIdx;
      float x = static_cast<float>(logicalLane) * effectiveLaneWidth;

      Transform t;
      t.scale = {effectiveLaneWidth, 50.0f, 10.1f};
      t.rotate = {0.0f, 0.0f, 0.0f};
      t.translate = {x, -23.0f, currentZs[zIndex]};

      renderObj->SetTransform(t);
      renderObj->Update(view, 0.0f);

      roadChunks_[zIndex][laneIdx] = renderObj;
      roadTransforms_[zIndex][laneIdx] = t;

      if (manager_)
        manager_->AddObject(renderObj);
    }
  }

  // サイドプレーンの生成/更新
  float effectiveLaneWidth = GetEffectiveLaneWidth();
  float bounds[2] = {static_cast<float>(minLaneIndex_) * effectiveLaneWidth -
                         (effectiveLaneWidth / 2.0f),
                     static_cast<float>(maxLaneIndex_) * effectiveLaneWidth +
                         (effectiveLaneWidth / 2.0f)};
  float offsets[2] = {-50.0f, 50.0f};
  const char *planeNames[2] = {"SidePlaneL", "SidePlaneR"};

  for (int i = 0; i < 2; ++i) {
    if (!sidePlanes_[i]) {
      auto model = std::make_shared<Model>();
      model->Initialize(planeModelData_);
      model->SetShader(WaterShader);
      model->SetColor({0.15f, 0.55f, 0.85f, 0.9f});
      model->SetLighting(true);
      model->SetCullMode(kCullModeNone);
      if (auto matComp = model->GetComponent<MaterialComponent>()) {
        matComp->SetTexturePath("Resources/Texture/white64x64.png");
        matComp->SetShader(WaterShader);
      }
      model->SetTexture(
          texture_->TextureData("Resources/Texture/white64x64.png"));
      sidePlanes_[i] = std::make_shared<RenderObject>(model);
      sidePlanes_[i]->SetName(planeNames[i]);
      if (manager_)
        manager_->AddObject(sidePlanes_[i]);
    }
    Transform t;
    t.scale = {
        50.0f, chunkLength_ * 2.0f,
        1.0f}; // plane.objは2x2なので、Yスケール*2=長さ。4チャンク分=chunkLength_*4
               // -> scale=chunkLength_*2
    t.rotate = {-1.570796f, 0.0f, 0.0f};
    t.translate = {bounds[i] + offsets[i], 0.0f,
                   chunkLength_ *
                       1.5f}; // カメラの手前から奥までカバーするように配置
    sidePlanes_[i]->SetTransform(t);
    sidePlanes_[i]->Update(view, 0.0f);
  }
}

void StageSettings::Update(Matrix4x4 view, float timeScale) {
  if (isDirty_) {
    GenerateRoadChunks(view);
    isDirty_ = false;
  }

  if (isGameOver_)
    return;

  // スクロール速度の加速（最大速度まで徐々に上がる）
  if (scrollSpeed_ < maxScrollSpeed_) {
    scrollSpeed_ += scrollAcceleration_ * timeScale;
    if (scrollSpeed_ > maxScrollSpeed_) {
      scrollSpeed_ = maxScrollSpeed_;
    }
  }
  if (scrollSpeed_ > maxScrollSpeed_ / 2) {
    // PostEffect::SetActivePostEffect(PostEffect::Type::Vignetting);
  } else {
    // PostEffect::SetActivePostEffect(PostEffect::Type::Normal);
  }

  float currentScroll = scrollSpeed_ * timeScale;

  // 道路チャンクのスクロール
  for (int i = 0; i < kChunkCount_; i++) {
    for (int laneIdx = 0; laneIdx < laneCount_; laneIdx++) {
      if (laneIdx < roadTransforms_[i].size()) {
        roadTransforms_[i][laneIdx].translate.z -= currentScroll;
      }
    }

    // 最も奥にあるチャンクのZ座標を探す (基準は0番レーンのZ)
    float maxZ = 0.0f;
    if (!roadTransforms_.empty() && !roadTransforms_[0].empty()) {
      maxZ = roadTransforms_[0][0].translate.z;
      for (int j = 1; j < kChunkCount_; j++) {
        if (!roadTransforms_[j].empty() &&
            roadTransforms_[j][0].translate.z > maxZ) {
          maxZ = roadTransforms_[j][0].translate.z;
        }
      }
    }

    // チャンクがカメラの後ろ（手前）を通り過ぎたら、一番奥に再配置
    if (!roadTransforms_[i].empty() &&
        roadTransforms_[i][0].translate.z < -chunkLength_) {
      float newChunkZ = maxZ + chunkLength_;
      for (int laneIdx = 0; laneIdx < laneCount_; laneIdx++) {
        if (laneIdx < roadTransforms_[i].size()) {
          roadTransforms_[i][laneIdx].translate.z = newChunkZ;
        }
      }
    }

    for (int laneIdx = 0; laneIdx < laneCount_; laneIdx++) {
      if (laneIdx < roadChunks_[i].size()) {
        roadChunks_[i][laneIdx]->SetTransform(roadTransforms_[i][laneIdx]);
      }
    }
  }

  // アイテムのクールタイム減算（実時間・秒単位）
  float dt = (1.0f / 60.0f) * timeScale;
  for (auto &pair : itemCoolDowns_) {
    if (pair.second.currentTimer > 0.0f) {
      pair.second.currentTimer -= dt;
      if (pair.second.currentTimer < 0.0f) {
        pair.second.currentTimer = 0.0f;
      }
    }
  }

  // 障害物・アイテムの定期生成
  distanceSinceLastSpawn_ += currentScroll;
  distanceSinceLastCameraItem_ += currentScroll;

  while (distanceSinceLastSpawn_ >= obstacleInterval_) {
    // 奥の固定位置(チャンクの向こう側)に生成
    if (!isSpawningPaused_) {
      SpawnObstacles(45.0f);
    }
    distanceSinceLastSpawn_ -= obstacleInterval_;

    // 次回生成までの間隔を移動速度＋猶予フレームに基づいて動的に再計算
    CalculateNextObstacleInterval();
  }

  // 障害物の更新
  for (int i = 0; i < kMaxObstacles_; i++) {
    obstacles_[i]->StageUpdate(view, currentScroll);
  }
}

void StageSettings::EditorUpdate(Matrix4x4 view) {
  if (isDirty_) {
    GenerateRoadChunks(view);
    isDirty_ = false;
  }

  // Editor中はスクロールさせないため、スピードを0として更新（WVPのみ更新させる）
  for (int i = 0; i < kMaxObstacles_; i++) {
    obstacles_[i]->StageUpdate(view, 0.0f);
  }
}

void StageSettings::Draw(class Draw &draw) {
  // 描画はGameObjectManagerが一括で行うため、ここでは何もしない
}

void StageSettings::SpawnObstacles(float z) {
  // === 狭まる区間の専用処理 ===
  if (isNarrowingSection_ && laneCount_ == 3) {
    // 左右レーンに GuideFloor を配置する
    obstacles_[nextObstacleIndex_]->SetType(Obstacle::Type::GuideFloor);
    obstacles_[nextObstacleIndex_]->Spawn(-laneWidth_, 2.0f, z);
    nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;

    obstacles_[nextObstacleIndex_]->SetType(Obstacle::Type::GuideFloor);
    obstacles_[nextObstacleIndex_]->Spawn(laneWidth_, 2.0f, z);
    nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;

    consecutiveNoSpawnCount_ =
        0; // 障害物が配置されたため連続空ウェーブをリセット
    return;
  }
  // =============================

  // === 時々障害物が出ない（空ウェーブ/安全区間）判定 ===
  bool isEmptyWave = false; // 障害物を一切生成しない空ウェーブフラグ
  if (consecutiveNoSpawnCount_ < maxConsecutiveNoSpawn_) {
    int roll = std::rand() % 100; // 0〜99の乱数
    int chancePercent =
        static_cast<int>(noSpawnChance_ * 100.0f); // 確率をパーセンテージに変換
    if (roll < chancePercent) {
      isEmptyWave = true;
      consecutiveNoSpawnCount_++; // 連続空ウェーブ回数をカウント
    } else {
      consecutiveNoSpawnCount_ = 0; // 障害物が出るためリセット
    }
  } else {
    consecutiveNoSpawnCount_ = 0; // 連続上限に達したためリセット
  }

  // たまにボーナスまたはアイテムを配置する（各アイテムのクールタイムを考慮）
  // ただし1レーンの場合は出さない
  int bonusLane =
      -1; // ボーナスまたはアイテムを配置するレーン番号（-1は配置なし）
  Obstacle::Type itemType = Obstacle::Type::Bonus; // アイテムの種類
  if (laneCount_ > 1) {
    int roll = std::rand() % 100;
    int spawnPercent = static_cast<int>(itemSpawnChance_ * 100.0f);
    if (roll < spawnPercent) {
      // クールタイムが終了している（currentTimer <=
      // 0.0f）アイテムを候補として収集
      std::vector<Obstacle::Type> availableItems;
      for (const auto &pair : itemCoolDowns_) {
        // 1レーン時はCameraItemは除外
        if (laneCount_ == 1 && pair.first == Obstacle::Type::CameraItem) {
          continue;
        }
        if (pair.second.currentTimer <= 0.0f) {
          availableItems.push_back(pair.first);
        }
      }

      // 候補が存在する場合のみアイテムを配置
      if (!availableItems.empty()) {
        bonusLane = std::rand() % laneCount_;
        int selectedIndex = std::rand() % availableItems.size();
        itemType = availableItems[selectedIndex];

        // 選ばれたアイテムのクールタイムを再設定（カウントダウン開始）
        itemCoolDowns_[itemType].currentTimer =
            itemCoolDowns_[itemType].duration;
      }
    }
  }

  // 空ウェーブの場合は障害物を配置せず、アイテムのみ生成（または完全な安全区間）
  if (isEmptyWave) {
    if (bonusLane != -1) {
      int lane = minLaneIndex_ + bonusLane; // アイテム配置対象のレーン番号
      float x = static_cast<float>(lane) * laneWidth_; // レーンのワールドX座標
      obstacles_[nextObstacleIndex_]->SetType(itemType);
      obstacles_[nextObstacleIndex_]->Spawn(x, 2.5f, z - 5.0f);
      nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;
    }
    return; // 障害物は出さずに終了
  }

  // レーンの状態を決定 (0: None, 1: Low, 2: High, 3: Wall, 4: Bonus)
  std::vector<int> laneSpawns(laneCount_);
  int wallCount = 0;
  int noneCount = 0;

  for (int i = 0; i < laneCount_; i++) {
    laneSpawns[i] = std::rand() % 4; // 0~3
    if (laneSpawns[i] == 3) {
      wallCount++;
    } else if (laneSpawns[i] == 0) {
      noneCount++;
    }
  }

  // すべて「何もない(None)」の場合は、最低1つの障害物を配置する
  if (noneCount == laneCount_) {
    int changeIndex = std::rand() % laneCount_;
    if (laneCount_ == 1) {
      // レーンが1つの場合はWall(3)を生成しないようにする(1:Low, 2:High)
      laneSpawns[changeIndex] = 1 + (std::rand() % 2);
    } else {
      laneSpawns[changeIndex] = 1 + (std::rand() % 3); // 1, 2, 3 のどれか
    }
  }

  // noneCountの処理でWallが増えた可能性があるのでwallCountを再計算
  wallCount = 0;
  for (int i = 0; i < laneCount_; i++) {
    if (laneSpawns[i] == 3) {
      wallCount++;
    }
  }

  // 全てWallの場合は1つを確実に通れるようにする
  if (wallCount == laneCount_) {
    int changeIndex = std::rand() % laneCount_;
    if (laneCount_ == 1) {
      // 1レーンしかなく全てWallの場合は、必ず通れる障害物にする
      laneSpawns[changeIndex] = 1 + (std::rand() % 2); // 1:Low または 2:High
    } else {
      laneSpawns[changeIndex] = 0; // Noneに変更
    }
  }

  // ボーナス配置時に障害物があるレーンを優先する処理（通常ウェーブ時）
  if (bonusLane != -1) {
    std::vector<int> obstacleLanes; // 障害物が存在するレーンのリスト
    for (int i = 0; i < laneCount_; i++) {
      if (laneSpawns[i] != 0) {
        obstacleLanes.push_back(i);
      }
    }
    if (!obstacleLanes.empty()) {
      bonusLane = obstacleLanes[std::rand() % obstacleLanes.size()];
    }
  }

  // 決定した内容で各レーンに生成
  for (int i = 0; i < laneCount_; i++) {
    if (laneSpawns[i] == 0 && i != bonusLane)
      continue; // None 且つ ボーナスも無いならスキップ

    int lane = minLaneIndex_ + i; // レーンインデックス（-1, 0, 1）
    float x = static_cast<float>(lane) * laneWidth_; // レーンのX座標

    // ボーナスまたはアイテムの生成（障害物の手前に配置）
    if (i == bonusLane) {
      obstacles_[nextObstacleIndex_]->SetType(itemType);
      obstacles_[nextObstacleIndex_]->Spawn(x, 2.5f, z - 5.0f);
      nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;
    }

    // 障害物の生成
    if (laneSpawns[i] != 0) {
      Obstacle::Type type = Obstacle::Type::Low; // 障害物タイプ
      float y = 2.5f;                            // 障害物のY座標

      if (laneSpawns[i] == 1) {
        type = Obstacle::Type::Low;
        y = 2.5f;
      } else if (laneSpawns[i] == 2) {
        type = Obstacle::Type::High;
        y = 4.6f;
      } else if (laneSpawns[i] == 3) {
        type = Obstacle::Type::Wall;
        y = 3.5f;
      }

      // 障害物のタイプを変更して配置
      obstacles_[nextObstacleIndex_]->SetType(type);
      obstacles_[nextObstacleIndex_]->Spawn(x, y, z);

      // 次のインデックスへ（リングバッファ的に使う）
      nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;
    }
  }
}

void StageSettings::Reset() {
  isGameOver_ = false;
  isNarrowingSection_ = false;
  isSpawningPaused_ = false;

  consecutiveNoSpawnCount_ = 0; // 連続空ウェーブ回数をリセット

  // スクロール速度を初期値にリセット
  scrollSpeed_ = baseScrollSpeed_;

  // 初回生成間隔の計算
  CalculateNextObstacleInterval();

  // 道路チャンクの位置をリセット
  for (int i = 0; i < kChunkCount_; i++) {
    for (int laneIdx = 0; laneIdx < laneCount_; laneIdx++) {
      if (laneIdx < roadTransforms_[i].size()) {
        roadTransforms_[i][laneIdx].translate.z =
            static_cast<float>(i) * chunkLength_;
        roadChunks_[i][laneIdx]->SetTransform(roadTransforms_[i][laneIdx]);
      }
    }
  }

  // 障害物を全て非アクティブに
  for (int i = 0; i < kMaxObstacles_; i++) {
    obstacles_[i]->Deactivate();
  }
  nextObstacleIndex_ = 0;
  // リセット後も間を空けずすぐに障害物が出現するように設定
  distanceSinceLastSpawn_ = (std::max)(0.0f, obstacleInterval_ - 3.0f);
  distanceSinceLastCameraItem_ = 0.0f;

  // アイテムのクールタイムをリセット（秒単位）
  itemCoolDowns_[Obstacle::Type::Bonus].currentTimer = 0.0f;
  itemCoolDowns_[Obstacle::Type::BarrierItem].currentTimer = 0.0f;
  itemCoolDowns_[Obstacle::Type::ClearItem].currentTimer = 0.0f;
  itemCoolDowns_[Obstacle::Type::CameraItem].currentTimer =
      10.0f; // 開始後すぐのカメラ変更を防ぐ猶予
  itemCoolDowns_[Obstacle::Type::BossItem].currentTimer =
      20.0f; // 開始後すぐのボス突入を防ぐ猶予
}

float StageSettings::GetItemCoolDownDuration(Obstacle::Type type) const {
  auto it = itemCoolDowns_.find(type);
  if (it != itemCoolDowns_.end()) {
    return it->second.duration;
  }
  return 0.0f;
}

void StageSettings::SetItemCoolDownDuration(Obstacle::Type type,
                                            float duration) {
  itemCoolDowns_[type].duration = (std::max)(0.0f, duration);
}

float StageSettings::GetItemCoolDownTimer(Obstacle::Type type) const {
  auto it = itemCoolDowns_.find(type);
  if (it != itemCoolDowns_.end()) {
    return it->second.currentTimer;
  }
  return 0.0f;
}

void StageSettings::SetItemCoolDownTimer(Obstacle::Type type, float timer) {
  itemCoolDowns_[type].currentTimer = (std::max)(0.0f, timer);
}

bool StageSettings::IsItemCoolDownReady(Obstacle::Type type) const {
  auto it = itemCoolDowns_.find(type);
  if (it != itemCoolDowns_.end()) {
    return it->second.currentTimer <= 0.0f;
  }
  return true;
}

