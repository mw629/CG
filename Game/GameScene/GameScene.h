#pragma once
#include "../../Editer/EditorUI.h"
#include "../IScene.h"
#include "Camera.h"
#include "GameObjectManager.h"
#include "Effect/GameSceneEffect.h"
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
	enum GameState { Title, Playing, Paused, PlayerHit, GameClear, GameOver, Editor };

	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera> camera_ = std::make_unique<Camera>();
	std::unique_ptr<Camera> gameCamera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f, 1.0f, 1.0f},{ 0.3f,0.0f,0.0f,},{0.0f, 8.0f, -15.0f} };
	Matrix4x4 view;

	// ゲーム状態
	GameState gameState_ = GameState::Title;

	// タイトル用
	std::unique_ptr<Sprite> titleSprite_;
	SpriteData titleSpriteData_;
	int titleTextureHandle_ = -1;

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
	float bossAttackSpawnZ_ = -35.0f; // ボスの攻撃生成Z座標 (奥から手前に向かってくる)
	float bossAttackDropHeight_ = 18.0f; // ボス攻撃が上空から降ってくる初期高さ
	float bossAttackFallDuration_ = 28.0f; // 落下にかかるフレーム数 (約0.45秒)
	float bossAttackReflectMinZ_ = -15.0f; // 反撃（跳ね返し）有効範囲の開始Z座標

	// ボス戦カメラ設定
	Vector3 bossCameraTranslate_{0.0f, 5.0f, 14.0f}; // ボス戦のカメラ位置 (少し下げた位置)
	Vector3 bossCameraRotate_{0.15f, 3.25f, 0.0f};    // ボス戦のカメラ角度 (角度を上げた/上向きにした角度)

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

	//<< Effects >>//
	std::unique_ptr<GameSceneEffect> effectManager_ =
		std::make_unique<GameSceneEffect>();

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
	void DrawHUD(class Draw& draw);
	void DrawTitleHUD(class Draw& draw);
	void DrawPlayingHUD(class Draw& draw);
	void DrawBossHUD(class Draw& draw);
	void DrawPauseHUD(class Draw& draw);
	void DrawGameOverHUD(class Draw& draw);
	void DrawControlsGuide(class Draw& draw);

	float uiTimer_ = 0.0f;

	void ChangePlayingState(PlayingState newState, bool force = false);

	void ResetGame();

	void TitleUpdate();

	void PlayingUpdate();

	void PausedUpdate();

	void PlayerHitUpdate();

	void EditorUpdate();

	class Draw* draw_ = nullptr;
};
