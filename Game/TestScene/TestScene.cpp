#include "TestScene.h"
#include <imgui.h>
#include <memory>

TestScene::~TestScene()
{
}

void TestScene::ImGui()
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

	//skyBox_.get()->ImGui();
	sphere_.get()->ImGui();
	//animation_.get()->ImGui();	
	//model_.get()->ImGui();
#endif // _USE_IMGUI
}

void TestScene::Initialize() {

	sceneID_ = SceneID::Test;

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	ModelData modelData = AssimpLoadObjFile("resources/obj", "sphere.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);


	ModelData animModel = AssimpLoadObjFile("resources/human", "sneakWalk.gltf");
	animation_.get()->Initialize(animModel, "resources/human", "sneakWalk.gltf");

	ModelData cubeModel = AssimpLoadObjFile("resources/AnimatedCube", "AnimatedCube.gltf");
	nodeAnimation_.get()->Initialize(cubeModel, "resources/AnimatedCube", "AnimatedCube.gltf");


	int texture1 = texture_.get()->CreateTexture("resources/monsterBall.png");
	sphere_.get()->Initialize(texture1);
	sphere_.get()->SetTransform(modelTransform_);

	skyBoxTexture_ = texture_.get()->CreateTexture("resources/rostock_laage_airport_4k.dds");
	skyBox_.get()->Initialize(skyBoxTexture_);
	skyBox_.get()->SetShader(ShaderName::SkyBoxShader);
	skyBox_.get()->SetLighting(false);
	skyBox_.get()->SetTransform(skyBoxTransform_);

	modelData = AssimpLoadObjFile("resources/Ground", "Ground.obj");
	floor.get()->Initialize(modelData);
	floor.get()->SetTransform(floorT);

	Emitter emitter;
	emitter.transform = modelTransform_;
	emitter.transform.scale = { 0.2f,0.2f,0.2f };
	emitter.count = 20;
	ParticleData data;
	data.transform.scale = { 0.05f,1.0f,1.0f };
	data.color = { 1.0f,0.0f,0.0f,1.0f };
	data.lifeTime = 1.0f;
	std::unique_ptr<ParticleManager> particle = std::make_unique<ParticleManager>();
	particle.get()->Initialize(emitter, data);

	// ランダムな角度(0度 or 45度など)で出現させ、その場から動かさない設定
	particle->generatorBehavior = [](ParticleData& p) {
		p.velocity = { 0.0f, 0.0f, 0.0f }; // 動かさない

		// 例として、Z軸の角度をランダムに設定 (0〜360度)
		float randomAngleDegrees = (float)(rand() % 360);
		p.transform.rotate.z = randomAngleDegrees * (3.14159265f / 180.0f);
	};

	particle_.push_back(std::move(particle));


	int texture = texture_.get()->CreateTexture("resources/uvChecker.png");
	sprite_.get()->Initialize(spriteData_, texture);
}

void TestScene::Update() {

	camera_.get()->Update();
	Matrix4x4 view = camera_.get()->GetViewMatrix();

	sprite_.get()->Update(spriteData_);

	model_->SettingWvp(view);
	floor->SettingWvp(view);

	sphere_.get()->SettingWvp(view);
	skyBox_.get()->SettingWvp(view);

	particle_[0].get()->Update(view);

	animation_.get()->Update(view);
	nodeAnimation_.get()->Update(view);
}

void TestScene::Draw() {

	Draw::SetCamera(camera_.get());
	// Set the SkyBox texture as environment map
	Draw::SetEnvironmentTexture(skyBoxTexture_);

	//Draw::DrawObj(skyBox_.get());
	//Draw::DrawObj(model_.get());
	//Draw::DrawObj(floor.get());
	//Draw::DrawObj(nodeAnimation_.get());
	Draw::DrawAnimation(animation_.get());

	//Draw::DrawObj(sphere_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->Draw();
	}
	//Draw::DrawSprite(sprite_.get());
}