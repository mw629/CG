#include "SceneManager.h"
#include "SceneGame.h"
#include "TitleScene.h"

SceneManager::SceneManager()
{
	scene_ = new SceneGame();
}

SceneManager::~SceneManager()
{
	delete scene_;
}

void SceneManager::ImGui() {
	scene_->ImGui();
}


void SceneManager::Initialize() {
	scene_->Initialize();
}

void SceneManager::Update() {
	scene_->Update();
}

void SceneManager::Draw() {
	scene_->Draw();
}
