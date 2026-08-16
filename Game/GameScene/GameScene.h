#pragma once
#include "../../Editer/EditorUI.h"
#include "../IScene.h"
#include "Camera.h"
#include "GameObjectManager.h"
#include "GameSceneParticle.h"
#include "RenderObject.h"
#include <Engine.h>
#include <Entity/Collision.h>
#include <Entity/Player.h>
#include <Entity/Boss.h>
#include <Stage/StageSettings.h>

#include <System/CollisionManager.h>
#include <System/PauseSystem.h>
#include <memory>


class GameScene : public IScene {
private:
	enum GameState { Playing, Paused, PlayerHit, GameClear, GameOver, Editor };

	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera> camera_ = std::make_unique<Camera>();
	std::unique_ptr<Camera> gameCamera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f, 1.0f, 1.0f},{ 0.3f,0.0f,0.0f,},{0.0f, 8.0f, -15.0f} };
	Matrix4x4 view;

	// ゲーム状態
	GameState gameState_ = GameState::Playing;

	// プレイ中の状態 (3レーン、1レーン、ボス)
	enum class PlayingState { ThreeLane, OneLane, Boss };
	PlayingState playingState_ = PlayingState::ThreeLane;

	// カメラ補間用変数
	bool isCameraTransitionPending_ = false;
	Transform pendingCameraTargetTransform_;
	int pendingLaneCount_ = 3;

	bool isCameraTransitioning_ = false;
	float cameraTransitionTimer_ = 0.0f;
	float cameraTransitionDuration_ = 1.0f; // 1秒で移動
	Transform startCameraTransform_;
	Transform targetCameraTransform_;

	// 右サイドカメラ（アイテム取得後）の管理
	bool isRightSideMode_ = false;
	float rightSideDistance_ = 0.0f;

	void StartCameraTransition(const Transform& targetTransform, int laneCount);
	void UpdateCameraTransition();

	//<<Common>>

	// スカイボックス
	std::shared_ptr<Cube> skyBox_ = std::make_shared<Cube>();
	int skyBoxTexture_;
	Transform skyBoxTransform_{
		{100.0f, 100.0f, 100.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	//<< Playing >>//
	// プレイヤー管理
	std::shared_ptr<Player> player_ = std::make_shared<Player>();
	// ボス管理
	std::shared_ptr<Boss> boss_ = std::make_shared<Boss>();
	float bossAttackTimer_ = 0.0f;

	// オブジェクト管理
	std::unique_ptr<GameObjectManager> gameObjectManager_ =
		std::make_unique<GameObjectManager>();
	// エディターUI
	std::unique_ptr<class EditorUI> editorUI_ = std::make_unique<EditorUI>();
	// ステージ管理
	std::unique_ptr<StageSettings> stageSettings_ =
		std::make_unique<StageSettings>();
	// コリジョン管理
	std::unique_ptr<class CollisionManager> collisionManager_;

	//<< Paused >>//
	std::unique_ptr<PauseSystem> pauseSystem_ = std::make_unique<PauseSystem>();

	//<< Particles >>//
	std::unique_ptr<GameSceneParticle> particleManager_ =
		std::make_unique<GameSceneParticle>();

	// 当たり判定処理
	void CheckCollisions();
	void CheckKeepRolling();

	// スコア・ランキング
	float currentDistance_ = 0.0f;
	float topRankings_[3] = { 0.0f, 0.0f, 0.0f };
	void UpdateRanking();

	float currentScore_ = 0.0f;
	float topScoreRankings_[3] = { 0.0f, 0.0f, 0.0f };
	int bonusEnemyHitCount_ = 0;
	void UpdateScoreRanking();

	// 初期読み込みするSceneJsonのファイルパス
	// (変更することで読み込むJsonを決定できる)
	std::string initialSceneJson_ = "Resources/Json/Scene/scene.json";

public:
	~GameScene() override;

	void ImGui() override;

	void Initialize() override;

	void Update() override;

	void Draw(class Draw& draw) override;

	void ChangePlayingState(PlayingState newState, bool force = false);

	void PlayingUpdate();

	void PausedUpdate();

	void PlayerHitUpdate();

	void EditorUpdate();
};
