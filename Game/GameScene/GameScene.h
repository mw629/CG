#pragma once
#include <Engine.h>
#include "Camera.h"
#include "../IScene.h"
#include "ParticleManager.h"
#include <memory>
#include <Player.h>

class GameScene :public IScene
{
private:
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,2.0f,-5.0f} };

	std::unique_ptr<Player>player_ = std::make_unique<Player>();



public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

