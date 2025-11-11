#include "GameScene.h"
#include <imgui.h>
#include "Map/MapManager.h"

namespace {
	int g_stage = 0;
}

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{

	switch (gameState_)
	{
	case kPlaying:
		playing_.get()->ImGui();
		break;
	case kPause:
		break;
	default:
		break;
	}

}

void GameScene::Initialize() {

	sceneID_ = SceneID::kGame;

	

	playing_ = std::make_unique<Playing>();
	playing_.get()->Initialize(g_stage);

	pause_ = std::make_unique<Pause>();
	//pause_.get()->Initialize();


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

	StateUpdate();

}

void GameScene::Draw() {

	StateDraw();
}


void GameScene::StateUpdate()
{
	switch (gameState_)
	{
	case kPlaying:
		playing_.get()->Update();

		break;
	case kPause:
		pause_.get()->Update();

		break;
	default:
		break;
	}

}

void GameScene::StateDraw()
{
	switch (gameState_)
	{
	case kPlaying:
		playing_.get()->Draw();
		break;
	case kPause:
		pause_.get()->Draw();
		break;
	default:
		break;
	}
}

void GameScene::ChangeState()
{
	switch (gameState_)
	{
	case kPlaying:
		if (Input::PushKey(DIK_ESCAPE)) {
			gameState_ = kPause;
		}

		break;
	case kPause:
		
		break;
	default:
		break;
	}
}

void GameScene::SetStage(int stage)
{
	g_stage = stage;
}

