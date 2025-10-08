#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>
#include "IScene.h"

class SceneGame :public IScene
{
private:

	std::unique_ptr<Camera>camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,-20.0f} };


	std::unique_ptr<Model> model_;
	Vector4 modelColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	int blendMode_ = int(BlendMode::kBlendModeNone);


public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

