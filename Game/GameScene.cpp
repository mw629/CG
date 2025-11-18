#include "GameScene.h"
#include <imgui.h>

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{

	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &cameraTransform_.translate.x);
		ImGui::DragFloat3("CameraSize", &cameraTransform_.scale.x);
		ImGui::DragFloat3("CameraRotate", &cameraTransform_.rotate.x);

	}

}

void GameScene::Initialize() {
	
	sceneID_ = SceneID::Game;

	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	for (int i = 0; i < 10; i++) {
		particleTransform_.push_back({ {1.0f,1.0f,1.0f},{0.0f ,0.0f,0.0f,},{0.0f + 0.1f * i,0.0f + 0.1f * i,0.0f} });
	}

	particle_.get()->Initialize(particleTransform_);

	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);
}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	model_->SettingWvp(camera_.get()->GetViewMatrix());
	particle_->SettingWvp(camera_.get()->GetViewMatrix());

}

void GameScene::Draw() {
	Draw::DrawModel(model_.get());
	Draw::DrawParticle(particle_.get());
	
}