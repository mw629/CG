#include "GameScene.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif
#include "Map/MapManager.h"

namespace {
	
}

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{



}

void GameScene::Initialize() {



}

void GameScene::Update() {

	if (Input::PushKey(DIK_Q)) {
		nextSceneID_ = SceneID::kTitle;
		sceneChangeRequest_ = true;
	}
	if (playing_.get()->IsGoal()) {
		nextSceneID_ = SceneID::kClear;
		sceneChangeRequest_ = true;
	}
	if (playing_.get()->IsGameOver()) {
		nextSceneID_ = SceneID::kGameOver;
		sceneChangeRequest_ = true;
	}

}

void GameScene::Draw() {

}


void GameScene::StateUpdate()
{


}

void GameScene::StateDraw()
{

}

void GameScene::ChangeState()
{
	
}

void GameScene::SetStage(int stage)
{

}

