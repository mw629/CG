#pragma once
#include <memory>
#include <vector>
#include <Engine.h>
#include <Entity/Obstacle.h>
#include "RenderObject.h"
#include "GameObjectManager.h"

/// <summary>
/// ステージの無限スクロールと障害物の管理を行うクラス
/// 道路チャンクを手前に移動させ、画面外に出たチャンクを奥に再配置する
/// </summary>
class StageSettings
{
private:

	// レーンの管理
	int minLaneIndex_ = -1;
	int maxLaneIndex_ = 1;
	float laneWidth_ = 2.0f;

	// スクロール速度
	float scrollSpeed_ = 0.2f;          // 現在のスクロール速度
	float baseScrollSpeed_ = 0.2f;      // 初期スクロール速度
	float maxScrollSpeed_ = 1.0f;       // 最大スクロール速度
	float scrollAcceleration_ = 0.001f; // 毎フレームの加速量

	// 道路チャンク
	static const int kChunkCount_ = 5;      // チャンクの数
	float chunkLength_ = 10.0f;              // 1チャンクの奥行き（Z軸方向のサイズ）

	std::shared_ptr<RenderObject> roadChunks_[kChunkCount_];
	Transform roadTransforms_[kChunkCount_];

	// テクスチャ
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	// 障害物管理
	static const int kMaxObstacles_ = 20;
	std::shared_ptr<Obstacle> obstacles_[kMaxObstacles_];
	int nextObstacleIndex_ = 0;

	float obstacleInterval_ = 30.0f; // 障害物の生成間隔
	float distanceSinceLastSpawn_ = 0.0f; // 前回生成からの移動距離

	// ゲームオーバーフラグ
	bool isGameOver_ = false;

	// 障害物をスポーンする
	void SpawnObstacles(float spawnZ);

public:

	void Initialize(ModelData roadModelData, ModelData obstacleModelData, ModelData bonusModelData, class GameObjectManager* manager);
	void Update(Matrix4x4 view, float timeScale = 1.0f);
	void EditorUpdate(Matrix4x4 view);
	void Draw();

	// ゲッター
	int GetMinLaneIndex() const { return minLaneIndex_; }
	int GetMaxLaneIndex() const { return maxLaneIndex_; }
	float GetLaneWidth() const { return laneWidth_; }
	float GetScrollSpeed() const { return scrollSpeed_; }
	float GetBaseScrollSpeed() const { return baseScrollSpeed_; }
	float GetMaxScrollSpeed() const { return maxScrollSpeed_; }
	float GetObstacleInterval() const { return obstacleInterval_; }

	// 設定
	void SetObstacleInterval(float interval) { obstacleInterval_ = interval; }

	// スクロール速度の設定
	void SetBaseScrollSpeed(float speed) { baseScrollSpeed_ = speed; scrollSpeed_ = speed; }
	void SetMaxScrollSpeed(float speed) { maxScrollSpeed_ = speed; }
	void SetScrollAcceleration(float accel) { scrollAcceleration_ = accel; }
	float GetScrollAcceleration() const { return scrollAcceleration_; }
	bool GetIsGameOver() const { return isGameOver_; }

	// ゲームオーバー設定
	void SetGameOver(bool isGameOver) { isGameOver_ = isGameOver; }

	// 障害物リストへのアクセス（当たり判定用）
	Obstacle* GetObstacle(int index) { return obstacles_[index].get(); }
	int GetMaxObstacles() const { return kMaxObstacles_; }

	// リセット
	void Reset();
};
