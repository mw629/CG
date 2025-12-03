#include "GameScene.h"
#include <imgui.h>
#include <memory>

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
#ifdef _USE_IMGUI
	
	camera_.get()->ImGui();
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->ImGui();
	}
	ImGui::DragFloat3("SpritePos", &spriteData_.transform.translate.x);

	model_.get()->ImGui();
#endif // _USE_IMGUI
}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;

	camera_.get()->Initialize();
	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	ModelData modelData = LoadObjFile("resources/obj", "sphere.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);

	modelData = LoadObjFile("resources/Ground", "Ground.obj");
	floor.get()->Initialize(modelData);
	floor.get()->SetTransform(floorT);

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

	texture = texture_.get()->CreateTexture("resources/uvChecker.png");
	sprite_.get()->Initialize(spriteData_,texture);
}

void GameScene::Update() {

	camera_.get()->Update();
	Matrix4x4 view = camera_.get()->GetViewMatrix();

	sprite_.get()->Update(spriteData_);

	model_->SettingWvp(view);
	floor->SettingWvp(view);

	particle_[1].get()->EmitSize();
	
	particle_[0].get()->Update(view, 0);
	particle_[1].get()->Update(view, 1);

}

void GameScene::Draw() {
	Draw::DrawModel(model_.get(), camera_.get());
	Draw::DrawModel(floor.get(), camera_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		//particle_[i].get()->Draw();
	}
	//Draw::DrawSprite(sprite_.get());
}