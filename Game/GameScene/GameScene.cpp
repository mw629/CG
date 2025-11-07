#include "GameScene.h"
#include <imgui.h>
#include "Map/MapManager.h"

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{

	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &cameraTransform_.translate.x);
		ImGui::DragFloat3("CameraSize", &cameraTransform_.scale.x);
		ImGui::DragFloat3("CameraRotate", &cameraTransform_.rotate.x);
	}
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

	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	playing_ = std::make_unique<Playing>();
	playing_.get()->Initialize(camera_.get()->GetViewMatrix());

	pause_ = std::make_unique<Pause>();
	//pause_.get()->Initialize();


}

void GameScene::Update() {

	if (Input::PushKey(DIK_Q)) {
		nextSceneID_ = SceneID::kTitle;
		sceneChangeRequest_ = true;
	}

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

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
		playing_.get()->Update(camera_.get()->GetViewMatrix());

		break;
	case kPause:
		pause_.get()->Update(camera_.get()->GetViewMatrix());

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

