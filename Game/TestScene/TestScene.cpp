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

	skyBox_.get()->ImGui();
	sphere_.get()->ImGui();
	animation_.get()->ImGui();	
	model_.get()->ImGui();
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
	skyBox_.get()->SetShader("SkyBoxShader");
	skyBox_.get()->SetLighting(false);
	skyBox_.get()->SetTransform(skyBoxTransform_);

	modelData = AssimpLoadObjFile("resources/Ground", "Ground.obj");
	floor.get()->Initialize(modelData);
	floor.get()->SetTransform(floorT);

    EmitterData emitter;
	emitter.transform = modelTransform_;
	emitter.transform.scale = { 0.1f,0.1f,0.1f };
	emitter.count = 5;
	EffectDefinitionData data;
	data.transform.scale = { 0.05f,1.0f,1.0f };
	data.color = { 1.0f,1.0f,1.0f,1.0f };
	data.lifeTime = 1.0f;
	// Plane particle (White) - Snow/Sparkles
	std::unique_ptr<Emitter> particlePlane = std::make_unique<Emitter>();
	EmitterData emitterPlane = emitter;
	emitterPlane.transform.translate.x -= 2.0f;
	emitterPlane.count = 2; // Spawn fewer per frame but steadily
	emitterPlane.frequency = 0.1f;
	EffectDefinitionData dataPlane = data;
	dataPlane.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // White
	dataPlane.lifeTime = 2.0f;
	dataPlane.transform.scale = { 0.1f, 0.1f, 0.1f };
	particlePlane.get()->Initialize(emitterPlane, dataPlane, EffectShape::Plane);
	particlePlane->generatorBehavior = [](EffectDefinitionData& p) {
		float randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
		float randY = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
		float randZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
		p.transform.translate.x += randX;
		p.transform.translate.y += randY;
		p.transform.translate.z += randZ;
		p.velocity = { randX * 0.01f, -0.02f, randZ * 0.01f }; // Falling down and drifting
		p.transform.rotate.z = ((float)rand() / RAND_MAX) * 3.14159f;
	};
	particle_.push_back(std::move(particlePlane));

	// Cylinder particle (Red) - Upward Fire Pillar / Laser
	std::unique_ptr<Emitter> particleCylinder = std::make_unique<Emitter>();
	EmitterData emitterCylinder = emitter;
	emitterCylinder.count = 3;
	emitterCylinder.frequency = 0.05f;
	EffectDefinitionData dataCylinder = data;
	dataCylinder.color = { 1.0f, 0.0f, 0.0f, 1.0f }; // Red
	dataCylinder.lifeTime = 0.8f;
	dataCylinder.transform.scale = { 0.15f, 1.5f, 0.15f }; // Long vertical shape
	particleCylinder.get()->Initialize(emitterCylinder, dataCylinder, EffectShape::Cylinder);
	particleCylinder->generatorBehavior = [](EffectDefinitionData& p) {
		float randX = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
		float randZ = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
		p.transform.translate.x += randX;
		p.transform.translate.z += randZ;
		p.velocity = { 0.0f, 0.08f + ((float)rand() / RAND_MAX) * 0.05f, 0.0f }; // Shoot straight up fast
	};
	particle_.push_back(std::move(particleCylinder));

	// Ring particle (Blue) - Expanding Shockwave
	std::unique_ptr<Emitter> particleRing = std::make_unique<Emitter>();
	EmitterData emitterRing = emitter;
	emitterRing.transform.translate.x += 2.0f;
	emitterRing.count = 1;
	emitterRing.frequency = 0.5f; // Infrequent spawn
	EffectDefinitionData dataRing = data;
	dataRing.color = { 0.0f, 0.5f, 1.0f, 1.0f }; // Cyan-ish Blue
	dataRing.lifeTime = 1.0f;
	dataRing.transform.scale = { 0.1f, 0.1f, 0.1f }; // Start small
	particleRing.get()->Initialize(emitterRing, dataRing, EffectShape::Ring);
	particleRing->generatorBehavior = [](EffectDefinitionData& p) {
		p.velocity = { 0.0f, 0.0f, 0.0f }; // Stay in place
	};
	particle_.push_back(std::move(particleRing));


	int texture = texture_.get()->CreateTexture("resources/uvChecker.png");
	sprite_.get()->Initialize(spriteData_, texture);

	ring_.get()->Initialize(texture);
   cylinder_.get()->Initialize(texture);
	cylinder_.get()->SetTransform(cylinderTransform_);
}

void TestScene::Update() {

	camera_.get()->Update();
	Matrix4x4 view = camera_.get()->GetViewMatrix();

	sprite_.get()->Update(spriteData_);
	ring_.get()->SettingWvp(view);
	cylinder_.get()->SettingWvp(view);

	model_->SettingWvp(view);
	floor->SettingWvp(view);

	sphere_.get()->SettingWvp(view);
	skyBox_.get()->SettingWvp(view);

	for (int i = 0; i < particle_.size(); ++i) {
		if (i == 2) { // Ring particle
			particle_[i].get()->Update(view, [](const EffectDefinitionData& p) {
				EffectDefinitionData next = p;
				// Expand scale continuously to create a shockwave
				next.transform.scale.x += 0.04f;
				next.transform.scale.y += 0.04f;
				next.transform.scale.z += 0.04f;
				return next;
			});
		} else {
			particle_[i].get()->Update(view);
		}
	}

	animation_.get()->Update(view);
	nodeAnimation_.get()->Update(view);
}

void TestScene::Draw() {

	Draw::SetCamera(camera_.get());
	// Set the SkyBox texture as environment map
	Draw::SetEnvironmentTexture(skyBoxTexture_);

	//Draw::DrawObj(ring_.get());
	//Draw::DrawObj(cylinder_.get());

	Draw::DrawObj(skyBox_.get());
	Draw::DrawObj(model_.get());
	Draw::DrawObj(floor.get());
	Draw::DrawObj(nodeAnimation_.get());
	Draw::DrawAnimation(animation_.get());

	//Draw::DrawObj(sphere_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		//particle_[i].get()->Draw();
	}
	//Draw::DrawSprite(sprite_.get());
}