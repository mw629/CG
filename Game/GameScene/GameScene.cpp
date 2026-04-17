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

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

}

void GameScene::Update() {

	
}

void GameScene::Draw() {

}