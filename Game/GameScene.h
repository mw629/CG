#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>
#include "IScene.h"
class GameScene :public IScene
{
private:

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,-20.0f} };

	std::unique_ptr<Particle> particle_ = std::make_unique<Particle>();
	std::vector<Transform> particleTransform_;

	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	Transform modelTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{6.0f,0.0f,0.0f} };

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

