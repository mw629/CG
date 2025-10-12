#include "GameScene.h"
#include <imgui.h>

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
	camera_.get()->ImGui();
	
	player_.get()->ImGui();

}

void GameScene::Initialize() {
	
	sceneID_ = SceneID::Game;

	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
	
	player_ = std::make_unique<Player>();
	player_.get()->Initialize();

}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	player_.get()->Update(camera_.get()->GetViewMatrix());
}

void GameScene::Draw() {

	player_.get()->Draw();

}