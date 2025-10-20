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

	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();



}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	


}

void GameScene::Draw() {

}