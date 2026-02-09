#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>
#include "IScene.h"
#include "GameScene/Entity/Player.h"
#include "GameScene/Entity/Enemy.h"

class GameScene :public IScene
{
private:

	std::unique_ptr<Camera>camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,-20.0f} };

	std::unique_ptr<Player> player_;

	//std::unique_ptr<Enemy> enemy_;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

