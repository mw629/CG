#pragma once
#include <Engine.h>
#include "Camera.h"
#include "IScene.h"
#include "ParticleManager.h"
#include <memory>

class GameScene :public IScene
{
private:
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,-50.0f} };

	std::vector<std::unique_ptr<ParticleManager>> particle_;
	Emitter particleEmitter_;

	std::unique_ptr<Sprite> sprite_ = std::make_unique<Sprite>();

	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	Transform modelTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,-2.0f,0.0f} };

	bool bill = true;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

