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

	ModelData modelData = LoadObjFile("resources/Stick", "Stick.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);

	Emitter emitter;
	emitter.transform = modelTransform_;
	emitter.transform.translate.y += 1.5f;
	emitter.transform.scale = { 0.2f,0.2f,0.2f };
	emitter.count = 20;
	ParticleData data;
	//int texture = texture_.get()->CreateTexture("resources/white64x64.png");
	data.transform.scale = { 0.5f,0.5f,0.5f };
	data.color = { 1.0f,0.0f,0.0f,1.0f };
	data.lifeTime = 1.0f;
	std::unique_ptr<ParticleManager> particle = std::make_unique<ParticleManager>();
	particle.get()->Initialize(emitter, data);
	particle_.push_back(std::move(particle));

	Emitter emitter1;
	emitter1.transform.translate.y += 2.0f;
	emitter1.transform.scale = { 4.0f,0.2f,0.2f };
	emitter1.count = 20;
	ParticleData data1;
	int texture = texture_.get()->CreateTexture("resources/white64x64.png");
	data1.transform.scale = { 0.01f,0.01f,0.01f };
	data1.lifeTime = 5.0f;
	std::unique_ptr<ParticleManager> particle1 = std::make_unique<ParticleManager>();
	particle1.get()->Initialize(emitter1, data1, texture);
	particle_.push_back(std::move(particle1));

	texture = texture_.get()->CreateTexture("resources/sousa.png");
	sprite_.get()->Initialize(texture);
	sprite_.get()->SetSize({ 0.0f,656.0f }, { 320.0f ,720.0f});
}

void GameScene::Update() {

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();
	Matrix4x4 view = camera_.get()->GetViewMatrix();

	sprite_.get()->SettingWvp();


	model_->SettingWvp(view);

	particle_[1].get()->EmitSize();
	
	particle_[0].get()->Update(view, 0);
	particle_[1].get()->Update(view, 1);

}

void GameScene::Draw() {
	Draw::DrawModel(model_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->Draw();
	}
	Draw::DrawSprite(sprite_.get());
}