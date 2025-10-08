#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>

class SceneGame
{
private:

	std::unique_ptr<Camera>camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,-20.0f} };


	std::unique_ptr<Model> model_;
	Vector4 modelColor_ = {1.0f,1.0f,1.0f,1.0f};
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	int blendMode_ = int(BlendMode::kBlendModeNone);

	std::unique_ptr<Model> model_2;
	Vector4 modelColor_2 = { 1.0f,1.0f,1.0f,1.0f };
	Transform transform_2 = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	int blendMode_2 = int(BlendMode::kBlendModeNone);

public:

	void ImGui();
	
	void Initialize();

	void Update();

	void Draw();

};

