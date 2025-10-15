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
	player_.get()->ImGui();

}

void GameScene::Initialize() {
	
	sceneID_ = SceneID::Game;

	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_.get()->SetMapData(MapManager::MapData(0));//stage数を引数に入れる

	mapchip_ = std::make_unique<MapChip>();
	mapchip_.get()->Initialize(mapChipField_.get(), camera_.get()->GetViewMatrix());

	player_ = std::make_unique<Player>();
	player_.get()->Initialize(mapChipField_.get()->GetPlayerSpawn(), camera_.get()->GetViewMatrix());
	player_.get()->SetMapChipField(mapChipField_.get());



}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	player_.get()->Update(camera_.get()->GetViewMatrix());

	mapchip_.get()->Update(camera_.get()->GetViewMatrix());


}

void GameScene::Draw() {

	player_.get()->Draw();
	mapchip_.get()->Draw();
}