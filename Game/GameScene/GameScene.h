#pragma once
#include <Engine.h>
#include "Camera.h"
#include "../IScene.h"
#include <memory>
#include <Entity/Player.h>

class GameScene :public IScene
{
private:
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f,},{0.0f,2.0f,-5.0f} };

	std::unique_ptr<Player>player_ = std::make_unique<Player>();

	std::unique_ptr<Model> road_ = std::make_unique<Model>();
	Transform roadT_{ {5.0f,5.0f,5.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,0.0f} };


public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

