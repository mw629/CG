#include "SceneManager.h"


void SceneManager::Initialize() {
	gameScene_ = std::make_unique<GameScene>();
	gameScene_.get()->Initialize();
}

void SceneManager::Update() {
	gameScene_.get()->Update();
}

void SceneManager::Draw() {
	gameScene_.get()->Draw();
}
