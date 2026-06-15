#include "StageSettings.h"
#include <cstdlib>
#include <ctime>

void StageSettings::Initialize(ModelData roadModelData, ModelData obstacleModelData, class GameObjectManager* manager)
{
	// 乱数の初期化
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// 道路チャンクの初期化
	for (int i = 0; i < kChunkCount_; i++) {
        auto roadModel = std::make_shared<Model>();
		roadModel->Initialize(roadModelData);
		roadModel->SetTexture(texture_->TextureData("resources/Model/Ground/road.png"));

		roadChunks_[i] = std::make_shared<RenderObject>(roadModel);
        roadChunks_[i]->SetName("RoadChunk " + std::to_string(i));

		// Z軸方向に並べて配置（手前から奥へ）
		roadTransforms_[i].scale = { 5.0f, 5.0f, 5.0f };
		roadTransforms_[i].rotate = { 0.0f, 0.0f, 0.0f };
		roadTransforms_[i].translate = { 0.0f, 2.0f, static_cast<float>(i) * chunkLength_ };

		roadChunks_[i]->SetTransform(roadTransforms_[i]);
        if (manager) manager->AddObject(roadChunks_[i]);
	}

	// 障害物の初期化
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i] = std::make_shared<Obstacle>();
        obstacles_[i]->SetName("Obstacle " + std::to_string(i));

		// ランダムなタイプで初期化
		Obstacle::Type type = static_cast<Obstacle::Type>(std::rand() % 3);
		obstacles_[i]->Initialize(obstacleModelData, type);
        
        if (manager) manager->AddObject(obstacles_[i]);
	}
}

void StageSettings::Update(Matrix4x4 view)
{
	if (isGameOver_) return;

	// スクロール速度の加速（最大速度まで徐々に上がる）
	if (scrollSpeed_ < maxScrollSpeed_) {
		scrollSpeed_ += scrollAcceleration_;
		if (scrollSpeed_ > maxScrollSpeed_) {
			scrollSpeed_ = maxScrollSpeed_;
		}
	}
	if (scrollSpeed_ > maxScrollSpeed_ / 2) {
		PostEffect::SetActivePostEffect(PostEffect::Type::Vignetting);
	}
	else {
		PostEffect::SetActivePostEffect(PostEffect::Type::Normal);
	}

	// 道路チャンクのスクロール
	for (int i = 0; i < kChunkCount_; i++) {
		roadTransforms_[i].translate.z -= scrollSpeed_;

		// 最も奥にあるチャンクのZ座標を探す
		float maxZ = roadTransforms_[0].translate.z;
		for (int j = 1; j < kChunkCount_; j++) {
			if (roadTransforms_[j].translate.z > maxZ) {
				maxZ = roadTransforms_[j].translate.z;
			}
		}

		// チャンクがカメラの後ろ（手前）を通り過ぎたら、一番奥に再配置
		if (roadTransforms_[i].translate.z < -chunkLength_) {
			float newChunkZ = maxZ + chunkLength_;
			roadTransforms_[i].translate.z = newChunkZ;

			maxZ = newChunkZ; // 次のチャンク計算のために更新
		}

		roadChunks_[i]->SetTransform(roadTransforms_[i]);
	}

	// 障害物の定期生成
	distanceSinceLastSpawn_ += scrollSpeed_;
	while (distanceSinceLastSpawn_ >= obstacleInterval_) {
		// 奥の固定位置(チャンクの向こう側)に生成
		SpawnObstacles(45.0f);
		distanceSinceLastSpawn_ -= obstacleInterval_;
	}

	// 障害物の更新
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->StageUpdate(view, scrollSpeed_);
	}
}

void StageSettings::Draw()
{
	// 描画はGameObjectManagerが一括で行うため、ここでは何もしない
}

void StageSettings::SpawnObstacles(float z)
{
	// 3レーンの状態を決定 (0: None, 1: Low, 2: High, 3: Wall)
	int laneSpawns[3];
	int wallCount = 0;
	int noneCount = 0;

	for (int i = 0; i < 3; i++) {
		laneSpawns[i] = std::rand() % 4; // 0~3
		if (laneSpawns[i] == 3) {
			wallCount++;
		} else if (laneSpawns[i] == 0) {
			noneCount++;
		}
	}

	// すべて「何もない(None)」の場合は、最低1つの障害物を配置する
	if (noneCount == 3) {
		int changeIndex = std::rand() % 3;
		laneSpawns[changeIndex] = 1 + (std::rand() % 3); // 1, 2, 3 のどれか
	}

	// 全てWallの場合は1つをNoneにする
	if (wallCount == 3) {
		int changeIndex = std::rand() % 3;
		laneSpawns[changeIndex] = 0; // Noneに変更
	}

	// 決定した内容で各レーンに生成
	for (int i = 0; i < 3; i++) {
		if (laneSpawns[i] == 0) continue; // None

		int lane = minLaneIndex_ + i; // -1, 0, 1
		float x = static_cast<float>(lane) * laneWidth_;

		Obstacle::Type type;
		float y = 2.5f;

		if (laneSpawns[i] == 1) {
			type = Obstacle::Type::Low;
			y = 2.5f;
		} else if (laneSpawns[i] == 2) {
			type = Obstacle::Type::High;
			y = 4.6f;
		} else { // 3
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

void StageSettings::Reset()
{
	isGameOver_ = false;

	// スクロール速度を初期値にリセット
	scrollSpeed_ = baseScrollSpeed_;

	// 道路チャンクの位置をリセット
	for (int i = 0; i < kChunkCount_; i++) {
		roadTransforms_[i].translate = { 0.0f, 2.0f, static_cast<float>(i) * chunkLength_ };
		roadChunks_[i]->SetTransform(roadTransforms_[i]);
	}

	// 障害物を全て非アクティブに
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->Deactivate();
	}
	nextObstacleIndex_ = 0;
	
	// リセット時は最初は少し進んでから障害物が出るようにする
	distanceSinceLastSpawn_ = obstacleInterval_ - 10.0f;
}