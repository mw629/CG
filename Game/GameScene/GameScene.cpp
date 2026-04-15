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
	// スライダーで座標を操作（表示・編集ともに小数第1桁）
	// 範囲は整数部4桁を許容する -9999.9 〜 9999.9
	ImGui::Spacing();
	ImGui::Text("SpritePos");
	ImGui::SliderFloat2("##sprite_pos_slider", &spriteData_.transform.translate.x, -9999.9f, 9999.9f, "%.1f");

	// 表示は整数部4桁・小数1桁風に（幅指定で揃える）
	ImGui::Text("Pos: %4.1f, %4.1f", spriteData_.transform.translate.x, spriteData_.transform.translate.y);

	model_.get()->ImGui();
#endif // _USE_IMGUI
}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	ModelData modelData = AssimpLoadObjFile("resources/obj", "sphere.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);


	ModelData animModel = AssimpLoadObjFile("resources/human", "sneakWalk.gltf");
	animation_.get()->Initialize(animModel, "resources/human", "sneakWalk.gltf");

	ModelData cubeModel = AssimpLoadObjFile("resources/AnimatedCube", "AnimatedCube.gltf");
	nodeanimation_.get()->Initialize(cubeModel, "resources/AnimatedCube", "AnimatedCube.gltf");


	int texture1 = texture_.get()->CreateTexture("resources/monsterBall.png");
	sphere_.get()->Initialize(texture1);
	sphere_.get()->SetTransform(modelTransform_);

	modelData = AssimpLoadObjFile("resources/Ground", "Ground.obj");
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
	sprite_.get()->Initialize(spriteData_, texture);
}

void GameScene::Update() {

	camera_.get()->Update();
	Matrix4x4 view = camera_.get()->GetViewMatrix();

	sprite_.get()->Update(spriteData_);

	model_->SettingWvp(view);
	floor->SettingWvp(view);

	sphere_.get()->SettingWvp(view);

	particle_[1].get()->EmitSize();

	particle_[0].get()->Update(view);
	particle_[1].get()->Update(view);

	animation_.get()->Update(view);
	nodeanimation_.get()->Update(view);
}

void GameScene::Draw() {

	Draw::SetCamera(camera_.get());

	Draw::DrawObj(model_.get());
	Draw::DrawObj(floor.get());
	//Draw::DrawObj(nodeanimation_.get());
	Draw::DrawAnimation(animation_.get());

	Draw::DrawObj(sphere_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->Draw();
	}
	//Draw::DrawSprite(sprite_.get());
}