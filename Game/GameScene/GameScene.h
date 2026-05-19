#pragma once
#include <Engine.h>
#include "Camera.h"
#include "../IScene.h"
#include <memory>
#include <Entity/Player.h>
#include <Entity/Collision.h>
#include <Stage/StageSettings.h>

class GameScene :public IScene
{
private:
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {0.5f,0.5f,0.5f},{0.4f,0.0f,0.0f,},{0.0f,10.0f,-15.0f} };

	std::unique_ptr<Player>player_ = std::make_unique<Player>();

	// ステージ管理
	std::unique_ptr<StageSettings> stageSettings_ = std::make_unique<StageSettings>();

	// ゲーム状態
	bool isGameOver_ = false;

	// 当たり判定処理
	void CheckCollisions();

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};
