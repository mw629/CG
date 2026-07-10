#include "StageSettings.h"
#include <cstdlib>
#include <ctime>

void StageSettings::Initialize(ModelData roadModelData, ModelData obstacleModelData, ModelData bonusModelData, class GameObjectManager* manager)
{
	// 乱数の初期化
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// グラウンドテクスチャをロード
	texture_->CreateTexture("Resources/Model/Ground/Ground.png");

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
        
        if (manager) manager->AddObject(obstacles_[i]);
	}
}

void StageSettings::GenerateRoadChunks(Matrix4x4 view)
{
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
	for (auto& row : roadChunks_) {
		for (auto& chunk : row) {
			if (manager_) manager_->RemoveObject(chunk);
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
				roadModel->SetTexture(texture_->TextureData("Resources/Model/Ground/Ground.png"));

				renderObj = std::make_shared<RenderObject>(roadModel);
			}

			renderObj->SetName("RoadChunk_" + std::to_string(zIndex) + "_Lane_" + std::to_string(laneIdx));

			int logicalLane = minLaneIndex_ + laneIdx;
			float x = static_cast<float>(logicalLane) * laneWidth_;

			Transform t;
			t.scale = { laneWidth_, 50.0f, 10.1f };
			t.rotate = { 0.0f, 0.0f, 0.0f };
			t.translate = { x, -23.0f, currentZs[zIndex] };

			renderObj->SetTransform(t);
			renderObj->Update(view, 0.0f);

			roadChunks_[zIndex][laneIdx] = renderObj;
			roadTransforms_[zIndex][laneIdx] = t;

			if (manager_) manager_->AddObject(renderObj);
		}
	}
}

void StageSettings::Update(Matrix4x4 view, float timeScale)
{
	if (isDirty_) {
		GenerateRoadChunks(view);
		isDirty_ = false;
	}

	if (isGameOver_) return;

	// スクロール速度の加速（最大速度まで徐々に上がる）
	if (scrollSpeed_ < maxScrollSpeed_) {
		scrollSpeed_ += scrollAcceleration_ * timeScale;
		if (scrollSpeed_ > maxScrollSpeed_) {
			scrollSpeed_ = maxScrollSpeed_;
		}
	}
	if (scrollSpeed_ > maxScrollSpeed_ / 2) {
		// PostEffect::SetActivePostEffect(PostEffect::Type::Vignetting);
	}
	else {
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
				if (!roadTransforms_[j].empty() && roadTransforms_[j][0].translate.z > maxZ) {
					maxZ = roadTransforms_[j][0].translate.z;
				}
			}
		}

		// チャンクがカメラの後ろ（手前）を通り過ぎたら、一番奥に再配置
		if (!roadTransforms_[i].empty() && roadTransforms_[i][0].translate.z < -chunkLength_) {
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

	// 障害物の定期生成
	distanceSinceLastSpawn_ += currentScroll;
	distanceSinceLastCameraItem_ += currentScroll;

	while (distanceSinceLastSpawn_ >= obstacleInterval_) {
		// 奥の固定位置(チャンクの向こう側)に生成
		if (!isSpawningPaused_) {
			// 500mを超えていて、かつ1レーンでない時に通常の障害物の代わりにアイテムを配置する
			if (distanceSinceLastCameraItem_ >= cameraItemInterval_ && laneCount_ != 1) {
				obstacles_[nextObstacleIndex_]->SetType(Obstacle::Type::CameraItem);
				obstacles_[nextObstacleIndex_]->Spawn(0.0f, 2.5f, 45.0f); // 中央レーンに生成
				nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;
				
				distanceSinceLastCameraItem_ -= cameraItemInterval_;
			} else {
				SpawnObstacles(45.0f);
			}
		}
		distanceSinceLastSpawn_ -= obstacleInterval_;
	}

	// 障害物の更新
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->StageUpdate(view, currentScroll);
	}
}

void StageSettings::EditorUpdate(Matrix4x4 view)
{
	if (isDirty_) {
		GenerateRoadChunks(view);
		isDirty_ = false;
	}

	// Editor中はスクロールさせないため、スピードを0として更新（WVPのみ更新させる）
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->StageUpdate(view, 0.0f);
	}
}

void StageSettings::Draw()
{
	// 描画はGameObjectManagerが一括で行うため、ここでは何もしない
}

void StageSettings::SpawnObstacles(float z)
{
	// === 狭まる区間の専用処理 ===
	if (isNarrowingSection_ && laneCount_ == 3) {
		// 左右レーンに GuideFloor を配置する
		obstacles_[nextObstacleIndex_]->SetType(Obstacle::Type::GuideFloor);
		obstacles_[nextObstacleIndex_]->Spawn(-laneWidth_, 2.0f, z);
		nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;

		obstacles_[nextObstacleIndex_]->SetType(Obstacle::Type::GuideFloor);
		obstacles_[nextObstacleIndex_]->Spawn(laneWidth_, 2.0f, z);
		nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;
		
		// 中央レーンは安全にするか、ボーナスを置く
		return; 
	}
	// =============================

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

	// たまにボーナスエネミーを配置する (約10%の確率)
	// ただし1レーンの場合はボーナスエネミーを出さない
	int bonusLane = -1;
	if (laneCount_ > 1 && std::rand() % 10 == 0) {
		// 障害物があるレーンを優先して選ぶ
		std::vector<int> obstacleLanes;
		for (int i = 0; i < laneCount_; i++) {
			if (laneSpawns[i] != 0) {
				obstacleLanes.push_back(i);
			}
		}

		if (!obstacleLanes.empty()) {
			bonusLane = obstacleLanes[std::rand() % obstacleLanes.size()];
		} else {
			bonusLane = std::rand() % laneCount_;
		}
	}

	// 決定した内容で各レーンに生成
	for (int i = 0; i < laneCount_; i++) {
		if (laneSpawns[i] == 0 && i != bonusLane) continue; // None 且つ ボーナスも無いならスキップ

		int lane = minLaneIndex_ + i; // -1, 0, 1
		float x = static_cast<float>(lane) * laneWidth_;

		// ボーナスエネミーの生成（障害物の手前に配置）
		if (i == bonusLane) {
			obstacles_[nextObstacleIndex_]->SetType(Obstacle::Type::Bonus);
			obstacles_[nextObstacleIndex_]->Spawn(x, 2.5f, z - 5.0f);
			nextObstacleIndex_ = (nextObstacleIndex_ + 1) % kMaxObstacles_;
		}

		// 障害物の生成
		if (laneSpawns[i] != 0) {
			Obstacle::Type type;
			float y = 2.5f;

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

void StageSettings::Reset()
{
	isGameOver_ = false;
	isNarrowingSection_ = false;

	// スクロール速度を初期値にリセット
	scrollSpeed_ = baseScrollSpeed_;

	// 道路チャンクの位置をリセット
	for (int i = 0; i < kChunkCount_; i++) {
		for (int laneIdx = 0; laneIdx < laneCount_; laneIdx++) {
			if (laneIdx < roadTransforms_[i].size()) {
				roadTransforms_[i][laneIdx].translate.z = static_cast<float>(i) * chunkLength_;
				roadChunks_[i][laneIdx]->SetTransform(roadTransforms_[i][laneIdx]);
			}
		}
	}

	// 障害物を全て非アクティブに
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->Deactivate();
	}
	nextObstacleIndex_ = 0;
	
	// リセット時は最初は少し進んでから障害物が出るようにする
	distanceSinceLastSpawn_ = obstacleInterval_ - 10.0f;
	distanceSinceLastCameraItem_ = 0.0f;
}