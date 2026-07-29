#pragma once
#include <Engine.h>
#include "Camera.h"
#include "../IScene.h"
#include <memory>
#include <Entity/Player.h>
#include <Entity/Collision.h>
#include <Stage/StageSettings.h>
#include <System/PauseSystem.h>
#include "GameObjectManager.h"
#include "RenderObject.h"
#include "../../Editer/EditorUI.h"
#include "GameSceneParticle.h"
#include <System/CollisionManager.h>
#include <GameObjects/Light/DirectionalLight.h>
#include <GameObjects/Light/PointLight.h>
#include <GameObjects/Light/SpotLight.h>

class GameScene :public IScene
{
private:

	enum GameState 
	{
		Playing,
		Paused,
		PlayerHit,
		GameClear,
		GameOver,
		Editor
	};


	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	std::unique_ptr<Camera>gameCamera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f,},{0.0f,8.0f,-15.0f} };
	Matrix4x4 view;

	// ゲーム状態
	GameState gameState_ = GameState::Playing;

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
	
	//スカイボックス
	std::shared_ptr<Cube> skyBox_ = std::make_shared<Cube>();
	int skyBoxTexture_;
	Transform skyBoxTransform_{ {100.0f,100.0f,100.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//<< Playing >>//
	// プレイヤー管理
	std::shared_ptr<Player> player_ = std::make_shared<Player>();
	// オブジェクト管理
	std::unique_ptr<GameObjectManager> gameObjectManager_ = std::make_unique<GameObjectManager>();
	// エディターUI
	std::unique_ptr<class EditorUI> editorUI_ = std::make_unique<EditorUI>();
	// ステージ管理
	std::unique_ptr<StageSettings> stageSettings_ = std::make_unique<StageSettings>();
	// コリジョン管理
	std::unique_ptr<class CollisionManager> collisionManager_;


	//<< Paused >>//
	std::unique_ptr<PauseSystem> pauseSystem_ = std::make_unique<PauseSystem>();

	//<< Particles >>//
	std::unique_ptr<GameSceneParticle> particleManager_ = std::make_unique<GameSceneParticle>();

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

	// 初期読み込みするSceneJsonのファイルパス (変更することで読み込むJsonを決定できる)
	std::string initialSceneJson_ = "Resources/Json/Scene/scene.json";

	//<< Lighting & Bloom setup >>//
	std::shared_ptr<DirectionalLight> mainLight_;
	std::shared_ptr<PointLight> playerPointLight_;
	std::shared_ptr<SpotLight> stageSpotLight_;

	//<< Post Effects Management >>//
	bool autoPostEffectEnabled_ = true; // true: ゲーム状態に応じて全16種を動的自動適用, false: ImGui手動指定
	float hitGlitchTimer_ = 0.0f;       // PlayerHit時のRandomShader (Glitch)タイマー
	float dissolveTimer_ = 0.0f;        // GameOver移行時のDissolveShaderタイマー
	bool enableRetroPixelMode_ = false; // レトロモード (PixelateShader)

	// ImGui手動オーバーライド用
	int manualPass0Index_ = 0;
	int manualPass1Index_ = 0;

	// ポストエフェクトの動的状態同期更新
	void UpdatePostEffects();

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw(class Draw& draw)override;

	void SetCameraToRightSide();
	void SetCameraToBehind();

	void PlayingUpdate();
	
	void PausedUpdate();
	
	void PlayerHitUpdate();

	void EditorUpdate();

};
