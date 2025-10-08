#include "SceneGame.h"
#include <imgui.h>

void SceneGame::ImGui()
{
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &cameraTransform_.translate.x, 0.1f);
		ImGui::DragFloat3("CameraSize", &cameraTransform_.scale.x, 0.1f);
		ImGui::DragFloat3("CameraRoteta", &cameraTransform_.rotate.x, 0.1f);
	}
	if (ImGui::CollapsingHeader("player1")) {
		ImGui::DragFloat3("Pos", &transform_.translate.x, 0.1f);
		ImGui::DragFloat3("Size", &transform_.scale.x, 0.1f);
		ImGui::DragFloat3("Roteta", &transform_.rotate.x, 0.1f);
		ImGui::ColorEdit4("Color", &modelColor_.x);
		ImGui::SliderInt("blendmode", &blendMode_, 0.0f, int(BlendMode::kBlendNum));
		model_.get()->SetBlend(BlendMode(blendMode_));
	}
}

void SceneGame::Initialize() {
	
	sceneID_ = SceneID::Game;

	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");

	model_ = std::make_unique<Model>();
	model_->Initialize(modelData);
	model_->CreateModel();
	model_.get()->GetMatrial()->SetColor(modelColor_);

	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
}

void SceneGame::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	model_.get()->GetMatrial()->SetColor(modelColor_);
	model_->SetTransform(transform_);
	model_->SettingWvp(camera_.get()->GetViewMatrix());

}

void SceneGame::Draw() {
	Draw::DrawObj(model_.get());
}