#include "GameScene.h"
#include <imgui.h>

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
	if (ImGui::CollapsingHeader("Particle")) {
		for (int i = 0; i < particleData_.size(); i++) {

			// 折りたたみ
			std::string label = "Particle " + std::to_string(i);
			if (ImGui::CollapsingHeader(label.c_str())) {

				ParticleData& p = particleData_[i];

				// Transform
				ImGui::Text("Transform");

				ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(),
					&p.transform.translate.x, 0.01f);

				ImGui::DragFloat3(("Rotation##" + std::to_string(i)).c_str(),
					&p.transform.rotate.x, 0.01f);

				ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(),
					&p.transform.scale.x, 0.01f);

				// Color
				ImGui::ColorEdit4(("Color##" + std::to_string(i)).c_str(),
					&p.color.x);
			}
		}
	}
	
	ImGui::Checkbox("bill", &bill);
	particle_.get()->SetBillboard(bill);

}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;

	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	for (int i = 0; i < 10; i++) {
		ParticleData particleData;
		particleData.transform = { {1.0f,1.0f,1.0f},{0.0f ,0.0f,0.0f,},{0.0f + 0.1f * i,0.0f + 0.1f * i,0.0f} };
		particleData.color = { 1.0f,1.0f,1.0f,1.0f };
		particleData_.push_back(particleData);
	}

	particle_.get()->Initialize(particleData_);

	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);
}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	model_->SettingWvp(camera_.get()->GetViewMatrix());

	particle_.get()->Updata(camera_.get()->GetViewMatrix(), particleData_);

}

void GameScene::Draw() {
	Draw::DrawModel(model_.get());
	Draw::DrawParticle(particle_.get());

}