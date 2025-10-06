#include "GameScene.h"
#include <imgui.h>

void GameScene::ImGui()
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
	if (ImGui::CollapsingHeader("player2")) {
		ImGui::DragFloat3("2Pos", &transform_2.translate.x, 0.1f);
		ImGui::DragFloat3("2Size", &transform_2.scale.x, 0.1f);
		ImGui::DragFloat3("2Roteta", &transform_2.rotate.x, 0.1f);
		ImGui::ColorEdit4("2Color", &modelColor_2.x);
		ImGui::SliderInt("2blendmode", &blendMode_2, 0.0f, int(BlendMode::kBlendNum));
		model_2.get()->SetBlend(BlendMode(blendMode_2));
	}
}

void GameScene::Initialize() {
	
	ModelData modelData = LoadObjFile("resources/fence", "fence.obj");
	ModelData modelDat = LoadObjFile("resources/fence", "fence.obj");


	model_ = std::make_unique<Model>();
	model_->Initialize(modelData);
	model_->CreateModel();
	model_.get()->GetMatrial()->SetColor(modelColor_);

	modelData = LoadObjFile("resources/Player", "Player.obj");
	model_2 = std::make_unique<Model>();
	model_2->Initialize(modelData);
	model_2->CreateModel();
	model_2.get()->GetMatrial()->SetColor(modelColor_2);

	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	model_.get()->GetMatrial()->SetColor(modelColor_);
	model_->SetTransform(transform_);
	model_->SettingWvp(camera_.get()->GetViewMatrix());

	model_2.get()->GetMatrial()->SetColor(modelColor_2);
	model_2->SetTransform(transform_2);
	model_2->SettingWvp(camera_.get()->GetViewMatrix());
}

void GameScene::Draw() {
	Draw::DrawObj(model_.get());
	Draw::DrawObj(model_2.get());
}