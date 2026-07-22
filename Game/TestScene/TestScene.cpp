#include "TestScene.h"
#include "Graphics/Render/Draw.h"
#include <imgui.h>
#include <memory>
#include <cmath>

TestScene::~TestScene()
{
}

void TestScene::ImGui()
{
#ifdef _USE_IMGUI
	ImGui::Begin("TestScene");

	camera_.get()->ImGui();
	
	// スライダーで座標を操作（表示・編集ともに小数第1桁）
	// 範囲は整数部4桁を許容する -9999.9 〜 9999.9
	if (ImGui::CollapsingHeader("Sprite Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Spacing();
		ImGui::Text("SpritePos");
		ImGui::SliderFloat2("##sprite_pos_slider", &spriteData_.transform.translate.x, -9999.9f, 9999.9f, "%.1f");

		// 表示は整数部4桁・小数1桁風に（幅指定で揃える） 
		ImGui::Text("Pos: %4.1f, %4.1f", spriteData_.transform.translate.x, spriteData_.transform.translate.y);
	}

	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->ImGui();
	}

	ImGui::End();

	if (editorUI_ && gameObjectManager_) {
		editorUI_->Draw(gameObjectManager_.get(), camera_->GetViewMatrix(), camera_->GetProjectionMatrix());
	}

#endif // _USE_IMGUI
}

void TestScene::Initialize() {

	sceneID_ = SceneID::Test;

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	ModelData modelData = AssimpLoadObjFile("Resources/Model/obj", "sphere.obj");

	//Modelの初期化
	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);
	model_.get()->name_ = "Sphere Model";


	//Animationの初期化
	ModelData animModel = AssimpLoadObjFile("Resources/gltf/human", "sneakWalk.gltf");
	animation_.get()->Initialize(animModel, "Resources/gltf/human", "sneakWalk.gltf");
	animation_.get()->name_ = "Animation Model";
	animation_.get()->SetVisibleBones(true); // ボーンを表示

	//NoodeAnimationの初期化
	ModelData cubeModel = AssimpLoadObjFile("Resources/AnimatedCube", "AnimatedCube.gltf");
	nodeAnimation_.get()->Initialize(cubeModel, "Resources/AnimatedCube", "AnimatedCube.gltf");
	nodeAnimation_.get()->name_ = "Node Animation Cube";

	
	//Sphereの初期化
	int texture1 = texture_.get()->CreateTexture("Resources/Texture/monsterBall.png");
	sphere_.get()->Initialize(texture1);
	sphere_.get()->SetTransform(modelTransform_);
	sphere_.get()->name_ = "MonsterBall Sphere";

	//SkyBoxの初期化
	skyBoxTexture_ = texture_.get()->CreateTexture("Resources/DDS/rostock_laage_airport_4k.dds");
	skyBox_.get()->Initialize(skyBoxTexture_);
	skyBox_.get()->SetShader("SkyBoxShader");
	skyBox_.get()->SetLighting(false);
	skyBox_.get()->SetTransform(skyBoxTransform_);
	skyBox_.get()->name_ = "SkyBox";

	modelData = AssimpLoadObjFile("Resources/Model/Ground", "Ground.obj");
	floor.get()->Initialize(modelData);
	floor.get()->SetTransform(floorT);
	floor.get()->name_ = "Floor";

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
	particlePlane.get()->name_ = "Particle (Snow/Sparkles)";
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
	particleCylinder.get()->name_ = "Particle (Upward Fire)";
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
	particleRing.get()->name_ = "Particle (Shockwave)";
	particleRing->generatorBehavior = [](EffectDefinitionData& p) {
		p.velocity = { 0.0f, 0.0f, 0.0f }; // Stay in place
	};
	particle_.push_back(std::move(particleRing));


	int texture = texture_.get()->CreateTexture("Resources/Texture/uvChecker.png");
	sprite_.get()->Initialize(spriteData_, texture);

	ring_.get()->Initialize(texture);
	cylinder_.get()->Initialize(texture);
	cylinder_.get()->SetTransform(cylinderTransform_);
    
	if (gameObjectManager_) {
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(model_));
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(animation_));
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(nodeAnimation_));
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(sphere_));
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(skyBox_));
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(floor));
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(ring_));
		gameObjectManager_->AddObject(std::make_shared<RenderObject>(cylinder_));
	}
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

	Transform animationTransform = animation_->GetTransform();

	Vector3 moveInput = GamePadInput::GetLeftStick();
	Vector3 moveDirection = { moveInput.x, 0.0f, moveInput.y };

	if (Input::PressKey(DIK_D) || Input::PressKey(DIK_RIGHT)) moveDirection.x += 1.0f;
	if (Input::PressKey(DIK_A) || Input::PressKey(DIK_LEFT))  moveDirection.x -= 1.0f;
	if (Input::PressKey(DIK_W) || Input::PressKey(DIK_UP))    moveDirection.z += 1.0f;
	if (Input::PressKey(DIK_S) || Input::PressKey(DIK_DOWN))  moveDirection.z -= 1.0f;

	float lengthSq = moveDirection.x * moveDirection.x + moveDirection.z * moveDirection.z;
	if (lengthSq > 0.0001f) {
		float length = std::sqrt(lengthSq);
		moveDirection.x /= length;
		moveDirection.z /= length;
		if (length > 1.0f) length = 1.0f;

		float speed = 0.05f;
		float moveDist = length * speed;
		animationTransform.translate.x += moveDirection.x * moveDist;
		animationTransform.translate.z += moveDirection.z * moveDist;

		animationTransform.rotate.y = std::atan2(moveDirection.x, moveDirection.z);
		animation_->SetTransform(animationTransform);

		float animSpeedScale = 1.0f;
		animation_.get()->UpdateWithDelta(view, moveDist * animSpeedScale);
	}
	else {
		animation_.get()->SetAnimationTime(0.0f);
		animation_.get()->UpdateWithDelta(view, 0.0f);
	}
	nodeAnimation_.get()->Update(view);
}

void TestScene::Draw(class Draw& draw) {

	draw.SetCamera(camera_.get());
	// Set the SkyBox texture as environment map
	draw.SetEnvironmentTexture(skyBoxTexture_);

	//draw.DrawObj(ring_.get());
	//draw.DrawObj(cylinder_.get());

	draw.DrawObj(skyBox_.get());
	//draw.DrawObj(model_.get());
	//draw.DrawObj(floor.get());
	//draw.DrawObj(nodeAnimation_.get());
	draw.DrawAnimation(animation_.get());

	//draw.DrawObj(sphere_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
	//	particle_[i].get()->Draw(draw);
	}
	//draw.DrawSprite(sprite_.get());
}