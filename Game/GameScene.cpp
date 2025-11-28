#include "GameScene.h"
#include <imgui.h>

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
#ifdef _USE_IMGUI
if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &cameraTransform_.translate.x);
		ImGui::DragFloat3("CameraSize", &cameraTransform_.scale.x);
		ImGui::DragFloat3("CameraRotate", &cameraTransform_.rotate.x);
	}
	camera_.get()->ImGui();

	particle_.get()->ImGui();
#endif // _USE_IMGUI
	
	

}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;

	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();


	particle_.get()->Initialize();

	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);
}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	model_->SettingWvp(camera_.get()->GetViewMatrix());

	particle_.get()->Update(camera_.get()->GetViewMatrix());

}

void GameScene::Draw() {
	Draw::DrawModel(model_.get());
	particle_.get()->Draw();

}