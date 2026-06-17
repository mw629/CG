#pragma once
#include <Engine.h>
#include "Camera.h"
#include "../IScene.h"
#include <memory>
#include <Entity/Player.h>
#include <Entity/Collision.h>
#include <Stage/StageSettings.h>
#include <System/PauseSystem.h>


class GameScene :public IScene
{
private:

	enum GameState 
	{
		Playing,
		Paused,
		GameClear,
		GameOver
	};


	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {0.5f,0.5f,0.5f},{0.4f,0.0f,0.0f,},{0.0f,10.0f,-15.0f} };
	Matrix4x4 view;

	// ゲーム状態
	GameState gameState_ = GameState::Playing;

	//<< Playing >>//
	// プレイヤー管理
	std::unique_ptr<Player>player_ = std::make_unique<Player>();
	// ステージ管理
	std::unique_ptr<StageSettings> stageSettings_ = std::make_unique<StageSettings>();

	//<< Paused >>//
	std::unique_ptr<PauseSystem> pauseSystem_ = std::make_unique<PauseSystem>();

	// デバッグ用ラインのプール
	std::vector<std::unique_ptr<Line>> debugLines_;

	// 当たり判定処理
	void CheckCollisions();

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;


	void PlayingUpdate();
	
	void PausedUpdate();



};
