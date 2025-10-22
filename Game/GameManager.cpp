#include "GameManager.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"

GameManager::GameManager()
{
	scene_ = new GameScene();
}

GameManager::~GameManager()
{
	delete scene_;
}

void GameManager::ImGui() {
	scene_->ImGui();
}

void GameManager::Initialize() {
	mapManager_ = std::make_unique<MapManager>();
	scene_->Initialize();
}

void GameManager::Update() {
	
	if (scene_->GetSceneChangeRequest()) {
		int NexrScene = scene_->GetNextSceneID();
		delete  scene_;
		scene_ = CreateScene(NexrScene);
		scene_->Initialize();
	}
	
	scene_->Update();
}

void GameManager::Draw() {
	scene_->Draw();
}

IScene* GameManager::CreateScene(int sceneID)
{
	switch (sceneID) {
	case SceneID::kTitle: return new TitleScene();
	case SceneID::kGame:  return new GameScene();
	default: return nullptr;
	}
}
