#include "GameScene.h"


void GameScene::Initialize() {
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");
	int index = texture->CreateTexture(modelData.material.textureDilePath);
	model_ = std::make_unique<Model>();
	model_->Initialize(modelData, texture->TextureData(index));
	model_->CreateModel();

	camera_ = std::make_unique<Camera>();
	camera_.get()->Initialize();

}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	model_->SetTransform(transform_);
	model_->SettingWvp(camera_.get()->GetViewMatrix());
}

void GameScene::Draw() {
	Draw::DrawObj(model_.get());
}