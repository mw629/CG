#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "TestScene.h"

SceneManager::SceneManager()
{
	// 最初のシーンをTestSceneに設定
	scene_ = std::make_unique<TestScene>();
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
		int NextScene = scene_->GetNextSceneID();
		scene_ = CreateScene(NextScene);
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
	case SceneID::Test:  return std::make_unique<TestScene>();
	case SceneID::Title: return std::make_unique<TitleScene>();
	case SceneID::Game:  return std::make_unique<GameScene>(); 
	default: return nullptr;
	}
}
