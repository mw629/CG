#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"

SceneManager::SceneManager()
{
	scene_ = std::make_unique<GameScene>();
	Initialize();
}

void SceneManager::ImGui() {
	scene_->ImGui();
}

void SceneManager::Initialize() {
	scene_->Initialize();
}

void SceneManager::Update() {
	
	if (scene_->GetSceneChangeRequest()) {
		int NexrScene = scene_->GetNextSceneID();
		scene_ = CreateScene(NexrScene);
		scene_->Initialize();
	}
	
	scene_->Update();
}

void SceneManager::Draw() {
	scene_->Draw();
}

void SceneManager::Run()
{
	ImGui();
	Update();
	Draw();
}

std::unique_ptr<IScene> SceneManager::CreateScene(int sceneID)
{
	switch (sceneID) {
	case SceneID::Title: return std::make_unique<TitleScene>();
	case SceneID::Game:  return std::make_unique<GameScene>();
	default: return nullptr;
	}
}
