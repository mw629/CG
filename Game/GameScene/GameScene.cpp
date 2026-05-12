#include "GameScene.h"
#include <imgui.h>
#include <memory>

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
#ifdef _USE_IMGUI

	camera_.get()->ImGui();

#endif // _USE_IMGUI
}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;

	camera_->SetTransform(cameraTransform_);
	camera_->Update();

	ModelData modelData = AssimpLoadObjFile("resources/player", "player.obj");
	player_->Initialize(modelData);


}

void GameScene::Update() {

	camera_->Update();
	Matrix4x4 view = camera_->GetViewMatrix();

	player_->Update(view);

}

void GameScene::Draw() {
	//カメラの設定
	Draw::SetCamera(camera_.get());
	//背景の設定
	Draw::SetEnvironmentTexture(texture_.get()->CreateTexture("resources/rostock_laage_airport_4k.dds"));

	player_->Draw();

}