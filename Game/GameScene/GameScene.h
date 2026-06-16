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

class GameScene :public IScene
{
private:

	enum GameState 
	{
		Playing,
		Paused,
		GameClear,
		GameOver,
		Editor
	};


	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {0.5f,0.5f,0.5f},{0.3f,0.0f,0.0f,},{0.0f,8.0f,-15.0f} };
	Matrix4x4 view;

	// ゲーム状態
	GameState gameState_ = GameState::Playing;

	//<<Common>>
	
	//スカイボックス
	std::shared_ptr<Cube> skyBox_ = std::make_shared<Cube>();
	int skyBoxTexture_;
	Transform skyBoxTransform_{ {500.0f,500.0f,500.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//<< Playing >>//
	// プレイヤー管理
	std::shared_ptr<Player> player_ = std::make_shared<Player>();
	// オブジェクト管理
	std::unique_ptr<GameObjectManager> gameObjectManager_ = std::make_unique<GameObjectManager>();
	// エディターUI
	std::unique_ptr<EditorUI> editorUI_ = std::make_unique<EditorUI>();
	// ステージ管理
	std::unique_ptr<StageSettings> stageSettings_ = std::make_unique<StageSettings>();


	//<< Paused >>//
	std::unique_ptr<PauseSystem> pauseSystem_ = std::make_unique<PauseSystem>();


	// 当たり判定処理
	void CheckCollisions();
	void CheckKeepRolling();

	// スコア・ランキング
	float currentDistance_ = 0.0f;
	float topRankings_[3] = { 0.0f, 0.0f, 0.0f };
	void UpdateRanking();

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;


	void PlayingUpdate();
	
	void PausedUpdate();
	
	void EditorUpdate();

};
