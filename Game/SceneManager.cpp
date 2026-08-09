#include "SceneManager.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "TestScene.h"
#include "JsonScene/JsonScene.h"
#include "ResultScene/ResultScene.h"
#include <Engine.h>
#include "../Editer/EditorManager.h"

SceneManager::SceneManager()
{
	// 最初のシーンをJsonSceneに設定
	scene_ = std::make_unique<GameScene>();
	Initialize();
}

void SceneManager::ImGui() {
	scene_->ImGui();
}

void SceneManager::Initialize() {
	scene_->Initialize();
}

void SceneManager::PreUpdate() {
	if (scene_->GetSceneChangeRequest()) {
		int NextScene = scene_->GetNextSceneID();
		scene_ = CreateScene(NextScene);
		scene_->Initialize();
	}
}

void SceneManager::Update() {
	
	scene_->Update();
}

void SceneManager::Draw(class Draw& draw) {
	scene_->Draw(draw);
}

void SceneManager::Run(class Draw& draw)
{
	ImGui();
	Update();
	Draw(draw);
}

std::unique_ptr<IScene> SceneManager::CreateScene(int sceneID)
{
	switch (sceneID) {
	case SceneID::Test:  return std::make_unique<TestScene>();
	case SceneID::Title: return std::make_unique<TitleScene>();
	case SceneID::Game:  return std::make_unique<GameScene>(); 
	case SceneID::Json:  return std::make_unique<JsonScene>();
	case SceneID::Clear: return std::make_unique<ResultScene>();
	default: return nullptr;
	}
}
