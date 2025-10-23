#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>
#include "../IScene.h"
#include "Playing.h"
#include "Pause.h"




class GameScene :public IScene
{
private:

	std::unique_ptr<Camera>camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{12.0f,0.0f,-50.0f} };

	std::unique_ptr<Playing> playing_;
	std::unique_ptr<Pause> pause_;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;


	//ゲーム用の関数

	void GameState();


	//ポーズ用の関数

	void PoseState();

	void PoseInput();

};

