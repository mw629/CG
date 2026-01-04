#include "GameScene.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif
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
		pause_.get()->ImGui();
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

	skyDome_ = std::make_unique<SkyDome>();
	skyDome_.get()->Initialize();

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


	ChangeState();
	StateUpdate();

	skyDome_.get()->Update();
}

void GameScene::Draw() {

	skyDome_.get()->Draw();

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
	playing_.get()->Draw();
	switch (gameState_)
	{
	case kPlaying:

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
			pause_.get()->Initialize();
		}

		break;
	case kPause:
		
		if (!pause_.get()->IsPause()) {
			gameState_ = kPlaying;
		}
		if (pause_.get()->IsSelect()) {
			nextSceneID_ = SceneID::kStageSelect;
			sceneChangeRequest_ = true;
		}

		break;
	default:
		break;
	}
}

void GameScene::SetStage(int stage)
{
	g_stage = stage;
}

