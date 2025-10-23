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

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	playing_.get()->Update(camera_.get()->GetViewMatrix());

}

void GameScene::Draw() {

	playing_.get()->Draw();
}


void GameScene::PoseState()
{
	PoseInput();


}

void GameScene::PoseInput()
{
	if (Input::PushKey(DIK_SPACE)) {
		if (menuCommand_ == kReturnToGame) {
			GameState_ = kGame;
		}
		else {
			sceneChangeRequest_ = true;

			switch (menuCommand_)
			{
			case GameScene::kRestart:
				nextSceneID_ = SceneID::kGame;
				sceneChangeRequest_ = true;
				break;
			case GameScene::kReturnToStageSelect:
				nextSceneID_ = SceneID::kStageSelect;
				sceneChangeRequest_ = true;
				break;
			}
		}
	}

	switch (menuCommand_)
	{
	case GameScene::kReturnToGame:
		if (Input::PushKey(DIK_S)) { menuCommand_ = kRestart; }
		break;
	case GameScene::kRestart:
		if (Input::PushKey(DIK_W)) { menuCommand_ = kReturnToGame; }
		if (Input::PushKey(DIK_S)) { menuCommand_ = kReturnToStageSelect; }
		break;
	case GameScene::kReturnToStageSelect:
		if (Input::PushKey(DIK_W)) { menuCommand_ = kRestart; }
		break;
	}


}


