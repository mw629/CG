#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>

class GameScene
{
private:

	std::unique_ptr<Camera>camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,-20.0f} };


	std::unique_ptr<Model> model_;
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

public:

	void Initialize();

	void Update();

	void Draw();

};

