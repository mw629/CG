#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>
#include "IScene.h"
class GameScene :public IScene
{
private:

	std::unique_ptr<Camera>camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{12.0f,0.0f,-50.0f} };

	std::unique_ptr<Particle> particle_;
	std::vector<Transform> particleTransform_;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

