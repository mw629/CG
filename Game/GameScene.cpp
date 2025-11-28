#include "GameScene.h"
#include <imgui.h>
#include <memory>

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
#ifdef _USE_IMGUI
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("CameraPos", &cameraTransform_.translate.x);
		ImGui::DragFloat3("CameraSize", &cameraTransform_.scale.x);
		ImGui::DragFloat3("CameraRotate", &cameraTransform_.rotate.x);
	}
	camera_.get()->ImGui();
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->ImGui();
	}
#endif // _USE_IMGUI
}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;

	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	for (int i = 0; i < 2; i++){
		std::unique_ptr<ParticleManager> particle = std::make_unique<ParticleManager>();
		particle.get()->Initialize();
		particle_.push_back(std::move(particle));
	}

	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);
}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();
	Matrix4x4 view = camera_.get()->GetViewMatrix();

	model_->SettingWvp(view);

	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->Update(view);
	}
}

void GameScene::Draw() {
	Draw::DrawModel(model_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->Draw();
	}
}