#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"

SceneManager::SceneManager()
{
	scene_ = new GameScene();
}

SceneManager::~SceneManager()
{
	delete scene_;
}

void SceneManager::ImGui() {
	scene_->ImGui();
}

void SceneManager::Initialize() {
	mapManager_ = std::make_unique<MapManager>();
	scene_->Initialize();
}

void SceneManager::Update() {
	
	if (scene_->GetSceneChangeRequest()) {
		int NexrScene = scene_->GetNextSceneID();
		delete  scene_;
		scene_ = CreateScene(NexrScene);
		scene_->Initialize();
	}
	
	scene_->Update();
}

void SceneManager::Draw() {
	scene_->Draw();
}

IScene* SceneManager::CreateScene(int sceneID)
{
	switch (sceneID) {
	case SceneID::Title: return new TitleScene();
	case SceneID::Game:  return new GameScene();
	default: return nullptr;
	}
}
