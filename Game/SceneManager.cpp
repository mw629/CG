#include "SceneManager.h"


void SceneManager::ImGui() {
	gameScene_.get()->ImGui();
}


void SceneManager::Initialize() {
	gameScene_ = std::make_unique<SceneGame>();
	gameScene_.get()->Initialize();
}

void SceneManager::Update() {
	gameScene_.get()->Update();
}

void SceneManager::Draw() {
	gameScene_.get()->Draw();
}
