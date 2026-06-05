#include "StageSettings.h"
#include <cstdlib>
#include <ctime>

void StageSettings::Initialize(ModelData roadModelData, ModelData obstacleModelData)
{
	// 乱数の初期化
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// 道路チャンクの初期化
	for (int i = 0; i < kChunkCount_; i++) {
		roadChunks_[i] = std::make_unique<Model>();
		roadChunks_[i]->Initialize(roadModelData);
		roadChunks_[i]->SetTexture(texture_->TextureData("resources/Model/Ground/road.png"));

		// Z軸方向に並べて配置（手前から奥へ）
		roadTransforms_[i].scale = { 5.0f, 5.0f, 5.0f };
		roadTransforms_[i].rotate = { 0.0f, 0.0f, 0.0f };
		roadTransforms_[i].translate = { 0.0f, 0.0f, static_cast<float>(i) * chunkLength_ };

		roadChunks_[i]->SetTransform(roadTransforms_[i]);
	}

	// 障害物の初期化
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i] = std::make_unique<Obstacle>();

		// ランダムなタイプで初期化
		Obstacle::Type type = static_cast<Obstacle::Type>(std::rand() % 3);
		obstacles_[i]->Initialize(obstacleModelData, type);
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

		// チャンクがカメラの後ろ（手前）を通り過ぎたら、一番奥に再配置
		if (roadTransforms_[i].translate.z < -chunkLength_) {
			// 最も奥にあるチャンクのZ座標を探す
			float maxZ = roadTransforms_[0].translate.z;
			for (int j = 1; j < kChunkCount_; j++) {
				if (roadTransforms_[j].translate.z > maxZ) {
					maxZ = roadTransforms_[j].translate.z;
				}
			}
			// 最も奥のチャンクの更に奥に配置
			roadTransforms_[i].translate.z = maxZ + chunkLength_;

			// 再配置したチャンクに障害物を配置
			SpawnObstaclesOnChunk(roadTransforms_[i].translate.z);
		}

		roadChunks_[i]->SetTransform(roadTransforms_[i]);
		roadChunks_[i]->SettingWvp(view);
	}

	// 障害物の更新
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->Update(view, scrollSpeed_);
	}
}

void StageSettings::Draw()
{
	// 道路チャンクの描画
	for (int i = 0; i < kChunkCount_; i++) {
		Draw::DrawObj(roadChunks_[i].get());
	}

	// 障害物の描画
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->Draw();
	}
}

void StageSettings::SpawnObstaclesOnChunk(float chunkZ)
{
	// チャンクに障害物を1～2個配置
	int obstacleCount = 1 + (std::rand() % 2);

	for (int i = 0; i < obstacleCount; i++) {
		// ランダムなレーンを選択
		int lane = minLaneIndex_ + (std::rand() % (maxLaneIndex_ - minLaneIndex_ + 1));
		float x = static_cast<float>(lane) * laneWidth_;
		float z = chunkZ + static_cast<float>(std::rand() % static_cast<int>(chunkLength_));

		// Y座標はタイプに応じて変える
		float y = 1.5f; // Lowの場合（地面の高さ1.0f + 障害物半分の高さ0.5f）
		Obstacle::Type type = obstacles_[nextObstacleIndex_]->GetType();
		if (type == Obstacle::Type::High) {
			y = 2.5f; // 高い障害物
		}
		else if (type == Obstacle::Type::Wall) {
			y = 2.5f; // 壁は高さ3.0なので中心は2.5f
		}

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
		roadTransforms_[i].translate = { 0.0f, 0.0f, static_cast<float>(i) * chunkLength_ };
		roadChunks_[i]->SetTransform(roadTransforms_[i]);
	}

	// 障害物を全て非アクティブに
	for (int i = 0; i < kMaxObstacles_; i++) {
		obstacles_[i]->Deactivate();
	}
	nextObstacleIndex_ = 0;
}