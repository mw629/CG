#pragma once
#include <Engine.h>
#include "Camera.h"
#include "IScene.h"
#include "ParticleManager.h"

class GameScene :public IScene
{
private:

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,-20.0f} };

	std::unique_ptr<ParticleManager> particle_ = std::make_unique<ParticleManager>();

	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	Transform modelTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{6.0f,0.0f,0.0f} };

	bool bill = true;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

