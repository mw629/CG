#include "TestScene.h"
#include <imgui.h>
#include <memory>

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

	skyBox_.get()->ImGui();
	sphere_.get()->ImGui();
	sphere2_.get()->ImGui();
	animation_.get()->ImGui();	
	model_.get()->ImGui();

	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		particle_[i].get()->ImGui();
	}

	if (ImGui::Button("Toggle Collision Draw")) {
		isDrawColi_ = !isDrawColi_;
	}
	ImGui::Text("Collision: %s", isCollision_ ? "TRUE" : "FALSE");

	ImGui::End();

	HapiColi::HapiColi::GetInstance().Update();

#endif // _USE_IMGUI
}

void TestScene::Initialize() {

	sceneID_ = SceneID::Test;

	camera_.get()->SetTransform(cameraTransform_);
	camera_.get()->Update();

	ModelData modelData = AssimpLoadObjFile("resources/Model/obj", "sphere.obj");

	model_.get()->Initialize(modelData);
	model_.get()->SetTransform(modelTransform_);
	model_.get()->name_ = "Sphere Model";


	ModelData animModel = AssimpLoadObjFile("resources/Model/human", "sneakWalk.gltf");
	animation_.get()->Initialize(animModel, "resources/Model/human", "sneakWalk.gltf");
	animation_.get()->name_ = "Animation Model";

	ModelData cubeModel = AssimpLoadObjFile("resources/AnimatedCube", "AnimatedCube.gltf");
	nodeAnimation_.get()->Initialize(cubeModel, "resources/AnimatedCube", "AnimatedCube.gltf");
	nodeAnimation_.get()->name_ = "Node Animation Cube";

	

	int texture1 = texture_.get()->CreateTexture("resources/Texture/monsterBall.png");
	sphere_.get()->Initialize(texture1);
	sphere_.get()->SetTransform(modelTransform_);
	sphere_.get()->name_ = "MonsterBall Sphere";

	sphere2_.get()->Initialize(texture1);
	sphere2_.get()->SetTransform(transform2_);
	sphere2_.get()->name_ = "Player Sphere 2";

	skyBoxTexture_ = texture_.get()->CreateTexture("resources/DDS/rostock_laage_airport_4k.dds");
	skyBox_.get()->Initialize(skyBoxTexture_);
	skyBox_.get()->SetShader("SkyBoxShader");
	skyBox_.get()->SetLighting(false);
	skyBox_.get()->SetTransform(skyBoxTransform_);
	skyBox_.get()->name_ = "SkyBox";

	contactSphere_->Initialize(skyBoxTexture_);
	normalBox_->Initialize(skyBoxTexture_);

	modelData = AssimpLoadObjFile("resources/Model/Ground", "Ground.obj");
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


	int texture = texture_.get()->CreateTexture("resources/Texture/uvChecker.png");
	sprite_.get()->Initialize(spriteData_, texture);

	ring_.get()->Initialize(texture);
   cylinder_.get()->Initialize(texture);
	cylinder_.get()->SetTransform(cylinderTransform_);

	HapiColi::HapiColi::GetInstance().Initialize();

	AABB initialModelAABB = Collision::MakeAABB(modelTransform_, 2.0f, 2.0f, 2.0f);
	initialModelAABB.min.y -= 1.0f;
	initialModelAABB.max.y -= 1.0f;
	Vector3 initialModelCenter = { (initialModelAABB.min.x + initialModelAABB.max.x)*0.5f, (initialModelAABB.min.y + initialModelAABB.max.y)*0.5f, (initialModelAABB.min.z + initialModelAABB.max.z)*0.5f };
	HapiColi::ObjectData baseA = HapiColi::ObjectData::CreateBox("Model_Box", {initialModelCenter.x, initialModelCenter.y, initialModelCenter.z}, {2.0f, 2.0f, 2.0f});

	AABB targetInitialAABB = Collision::MakeAABB(Transform_, 2.0f, 2.0f, 2.0f);
	targetInitialAABB.min.y -= 1.0f;
	targetInitialAABB.max.y -= 1.0f;
	Vector3 targetInitialCenter = { (targetInitialAABB.min.x + targetInitialAABB.max.x)*0.5f, (targetInitialAABB.min.y + targetInitialAABB.max.y)*0.5f, (targetInitialAABB.min.z + targetInitialAABB.max.z)*0.5f };
	HapiColi::ObjectData baseB = HapiColi::ObjectData::CreateBox("Target_Box", {targetInitialCenter.x, targetInitialCenter.y, targetInitialCenter.z}, {2.0f, 2.0f, 2.0f});
	
	HapiColi::HapiColi::GetInstance().RegisterFuzzTarget("Model vs Box Fuzzing", baseA, baseB, [](HapiColi::ObjectData& a, HapiColi::ObjectData& b) {
		AABB aabbA; 
		aabbA.min = {a.position.x - a.collider.size.x * 0.5f, a.position.y - a.collider.size.y * 0.5f, a.position.z - a.collider.size.z * 0.5f}; 
		aabbA.max = {a.position.x + a.collider.size.x * 0.5f, a.position.y + a.collider.size.y * 0.5f, a.position.z + a.collider.size.z * 0.5f};
		AABB aabbB;
		aabbB.min = {b.position.x - b.collider.size.x * 0.5f, b.position.y - b.collider.size.y * 0.5f, b.position.z - b.collider.size.z * 0.5f};
		aabbB.max = {b.position.x + b.collider.size.x * 0.5f, b.position.y + b.collider.size.y * 0.5f, b.position.z + b.collider.size.z * 0.5f};
		bool hit = Collision::CheckAABB(aabbA, aabbB);
		a.collision.isColliding = hit;
		b.collision.isColliding = hit;
	});
}

#include "../../HapiColi/HapiColiManager/PlaybackManager.h"
#include "../../HapiColi/HapiColiManager/Recorder.h"

void TestScene::Update() {
	HapiColi::HapiColi::GetInstance().BeginFrame(1.0f / 60.0f);

	auto playback = HapiColi::HapiColi::GetInstance().GetPlaybackManager();
	bool shouldUpdateScene = true;
	float timeScale = 1.0f;

	if (playback) {
		if (playback->IsReplayMode()) {
			shouldUpdateScene = false;
			
			// リプレイ中は記録されたデータを元にモデルの座標を上書きして、実際に動いているように見せる
			auto recorder = HapiColi::HapiColi::GetInstance().GetManager()->GetRecorder();
			if (recorder) {
				const auto& frames = recorder->GetRecordedFrames();
				int idx = playback->GetReplayFrameIndex();
				if (!frames.empty() && idx >= 0 && idx < frames.size()) {
					for (const auto& obj : frames[idx].objects) {
						if (obj.id == "Model_Box" || obj.id == "Model_Sphere") {
							modelTransform_.translate = { obj.position.x, obj.position.y, obj.position.z };
							model_.get()->SetTransform(modelTransform_);
						} else if (obj.id == "Target_Box") {
							Transform_.translate = { obj.position.x, obj.position.y, obj.position.z };
							sphere_.get()->SetTransform(Transform_);
						} else if (obj.id == "Sphere2") {
							transform2_.translate = { obj.position.x, obj.position.y, obj.position.z };
							sphere2_.get()->SetTransform(transform2_);
						}
					}
				}
			}
		} else {
			if (playback->IsSimulationPaused()) {
				shouldUpdateScene = playback->ConsumeStepRequest();
			}
			timeScale = playback->GetTimeScale();
		}
	}

	if (shouldUpdateScene) {
		// 移動処理 (model_ を操作)
		float moveSpeed = 0.1f * timeScale;
		if (Input::PressKey(DIK_W)) { modelTransform_.translate.z += moveSpeed; }
		if (Input::PressKey(DIK_S)) { modelTransform_.translate.z -= moveSpeed; }
		if (Input::PressKey(DIK_A)) { modelTransform_.translate.x -= moveSpeed; }
		if (Input::PressKey(DIK_D)) { modelTransform_.translate.x += moveSpeed; }
		model_.get()->SetTransform(modelTransform_);

		if (Input::PressKey(DIK_UP)) { transform2_.translate.z += moveSpeed; }
		if (Input::PressKey(DIK_DOWN)) { transform2_.translate.z -= moveSpeed; }
		if (Input::PressKey(DIK_LEFT)) { transform2_.translate.x -= moveSpeed; }
		if (Input::PressKey(DIK_RIGHT)) { transform2_.translate.x += moveSpeed; }
		sphere2_.get()->SetTransform(transform2_);
	}

	camera_.get()->Update();
	Matrix4x4 view = camera_.get()->GetViewMatrix();

	sprite_.get()->Update(spriteData_);
	ring_.get()->SettingWvp(view);
	cylinder_.get()->SettingWvp(view);

	model_->SettingWvp(view);
	floor->SettingWvp(view);

	sphere_.get()->SettingWvp(view);
	sphere2_.get()->SettingWvp(view);
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

	// 当たり判定 (Model_Box)
	AABB modelAABB = Collision::MakeAABB(modelTransform_, 2.0f, 2.0f, 2.0f);
	modelAABB.min.y -= 1.0f;
	modelAABB.max.y -= 1.0f;
	Vector3 modelCenter = { (modelAABB.min.x + modelAABB.max.x)*0.5f, (modelAABB.min.y + modelAABB.max.y)*0.5f, (modelAABB.min.z + modelAABB.max.z)*0.5f };
	Vector3 modelSize = { modelAABB.max.x - modelAABB.min.x, modelAABB.max.y - modelAABB.min.y, modelAABB.max.z - modelAABB.min.z };
	HapiColi::ObjectData modelBoxData = HapiColi::ObjectData::CreateBox(
		"Model_Box",
		{modelCenter.x, modelCenter.y, modelCenter.z},
		{modelSize.x, modelSize.y, modelSize.z}
	);

	// 当たり判定 (Model_Sphere)
	CollisionSphere modelSphere = Collision::MakeSphere(modelTransform_, 1.0f);
	modelSphere.center.y -= 1.0f;
	HapiColi::ObjectData modelSphereData = HapiColi::ObjectData::CreateSphere(
		"Model_Sphere",
		{modelSphere.center.x, modelSphere.center.y, modelSphere.center.z},
		modelSphere.radius
	);

	// 相手 (Box)
	AABB targetAABB = Collision::MakeAABB(Transform_, 2.0f, 2.0f, 2.0f);
	targetAABB.min.y -= 1.0f;
	targetAABB.max.y -= 1.0f;
	Vector3 targetCenter = { (targetAABB.min.x + targetAABB.max.x)*0.5f, (targetAABB.min.y + targetAABB.max.y)*0.5f, (targetAABB.min.z + targetAABB.max.z)*0.5f };
	Vector3 targetSize = { targetAABB.max.x - targetAABB.min.x, targetAABB.max.y - targetAABB.min.y, targetAABB.max.z - targetAABB.min.z };
	HapiColi::ObjectData targetBoxData = HapiColi::ObjectData::CreateBox(
		"Target_Box",
		{targetCenter.x, targetCenter.y, targetCenter.z},
		{targetSize.x, targetSize.y, targetSize.z}
	);

	bool hitBox = Collision::CheckAABB(modelAABB, targetAABB);
	bool hitSphere = Collision::CheckAABBSphere(targetAABB, modelSphere);

	AABB sphere2Col = Collision::MakeAABB(transform2_, 2.0f, 2.0f, 2.0f);
	sphere2Col.min.y -= 1.0f;
	sphere2Col.max.y -= 1.0f;
	Vector3 sphere2Center = { (sphere2Col.min.x + sphere2Col.max.x)*0.5f, (sphere2Col.min.y + sphere2Col.max.y)*0.5f, (sphere2Col.min.z + sphere2Col.max.z)*0.5f };
	Vector3 sphere2Size = { sphere2Col.max.x - sphere2Col.min.x, sphere2Col.max.y - sphere2Col.min.y, sphere2Col.max.z - sphere2Col.min.z };
	HapiColi::ObjectData sphere2Data = HapiColi::ObjectData::CreateBox(
		"Sphere2",
		{sphere2Center.x, sphere2Center.y, sphere2Center.z},
		{sphere2Size.x, sphere2Size.y, sphere2Size.z}
	);

	bool hitSphere2Box = Collision::CheckAABB(targetAABB, sphere2Col);
	bool hitSphere2Model = Collision::CheckAABB(modelAABB, sphere2Col);
	bool hitSphere2Sphere = Collision::CheckAABBSphere(sphere2Col, modelSphere);

	isCollision_ = hitBox || hitSphere || hitSphere2Box || hitSphere2Model || hitSphere2Sphere;
	
	HapiColi::HapiColi::GetInstance().UpdateFuzzTarget("Model vs Box Fuzzing", modelBoxData, targetBoxData);

	if (hitBox) {
		modelBoxData.SetCollision(targetBoxData.id);
		targetBoxData.SetCollision(modelBoxData.id);

		// 衝突点と法線の計算 (AABB vs AABB)
		Vector3 contactPoint = {
			(std::max)(modelAABB.min.x, (std::min)(targetAABB.max.x, modelAABB.max.x)),
			(std::max)(modelAABB.min.y, (std::min)(targetAABB.max.y, modelAABB.max.y)),
			(std::max)(modelAABB.min.z, (std::min)(targetAABB.max.z, modelAABB.max.z))
		};
		Vector3 contactNormal = { targetCenter.x - modelCenter.x, targetCenter.y - modelCenter.y, targetCenter.z - modelCenter.z };
		float distSq = contactNormal.x*contactNormal.x + contactNormal.y*contactNormal.y + contactNormal.z*contactNormal.z;
		if (distSq > 0.0001f) {
			float invDist = 1.0f / std::sqrt(distSq);
			contactNormal.x *= invDist;
			contactNormal.y *= invDist;
			contactNormal.z *= invDist;
		} else {
			contactNormal = { 0, 1, 0 };
		}

		modelBoxData.SetContactInfo({contactPoint.x, contactPoint.y, contactPoint.z}, {contactNormal.x, contactNormal.y, contactNormal.z});
		targetBoxData.SetContactInfo({contactPoint.x, contactPoint.y, contactPoint.z}, {-contactNormal.x, -contactNormal.y, -contactNormal.z});
	}

	if (hitSphere) {
		modelSphereData.SetCollision(targetBoxData.id);
		targetBoxData.SetCollision(modelSphereData.id);
	}

	if (hitSphere2Box) {
		sphere2Data.SetCollision(targetBoxData.id);
		targetBoxData.SetCollision(sphere2Data.id);
	}

	if (hitSphere2Model) {
		sphere2Data.SetCollision(modelBoxData.id);
		modelBoxData.SetCollision(sphere2Data.id);
	}

	if (hitSphere2Sphere) {
		sphere2Data.SetCollision(modelSphereData.id);
		modelSphereData.SetCollision(sphere2Data.id);
	}

	HapiColi::HapiColi::GetInstance().RecordObject(modelBoxData);
	HapiColi::HapiColi::GetInstance().RecordObject(modelSphereData);
	HapiColi::HapiColi::GetInstance().RecordObject(targetBoxData);
	HapiColi::HapiColi::GetInstance().RecordObject(sphere2Data);

	HapiColi::HapiColi::GetInstance().EndFrame();
}

void TestScene::Draw() {

	Draw::SetCamera(camera_.get());
	// Set the SkyBox texture as environment map
	Draw::SetEnvironmentTexture(skyBoxTexture_);

	//Draw::DrawObj(ring_.get());
	//Draw::DrawObj(cylinder_.get());

	Draw::DrawObj(skyBox_.get());
	Draw::DrawObj(model_.get());
	//Draw::DrawObj(floor.get());
	//Draw::DrawObj(nodeAnimation_.get());
	//Draw::DrawAnimation(animation_.get());
	Draw::DrawSphere(sphere_.get());
	Draw::DrawSphere(sphere2_.get());
	for (int i = 0, n = static_cast<int>(particle_.size()); i < n; ++i) {
		//particle_[i].get()->Draw();
	}
	//Draw::DrawSprite(sprite_.get());



	if (isDrawColi_) {
		HapiColi::HapiColi::GetInstance().BuildRenderCommands();
		const auto& renderCommands = HapiColi::HapiColi::GetInstance().GetRenderCommands();
		
		if (!renderCommands.empty()) {
			Draw::preDraw(LineShader, kBlendModeNormal);

			int lineIndex = 0;
			for (const auto& cmd : renderCommands) {
				if (lineIndex >= debugLines_.size()) {
					auto line = std::make_unique<Line>();
					line->CreateLine();
					debugLines_.push_back(std::move(line));
				}

				LineVertexData vertices[2] = {
					{ cmd.start, {cmd.color[0], cmd.color[1], cmd.color[2], cmd.color[3]} },
					{ cmd.end, {cmd.color[0], cmd.color[1], cmd.color[2], cmd.color[3]} }
				};
				debugLines_[lineIndex]->SetVertex(vertices);
				debugLines_[lineIndex]->SettingWvp(camera_->GetViewMatrix());
				Draw::DrawLine(debugLines_[lineIndex].get());
				lineIndex++;
			}

			// Clear the rest
			for (int i = lineIndex; i < debugLines_.size(); ++i) {
				LineVertexData vertices[2] = {
					{ {0,0,0}, {1,1,1,1} },
					{ {0,0,0}, {1,1,1,1} }
				};
				debugLines_[i]->SetVertex(vertices);
			}
		}

		const auto& solidCommands = HapiColi::HapiColi::GetInstance().GetSolidRenderCommands();
		for (const auto& scmd : solidCommands) {
			if (scmd.type == HapiColi::SolidRenderCommand::Sphere) {
				Transform t;
				t.scale = {scmd.scale.x, scmd.scale.y, scmd.scale.z};
				t.rotate = {scmd.rotation.x, scmd.rotation.y, scmd.rotation.z};
				t.translate = {scmd.position.x, scmd.position.y, scmd.position.z};
				contactSphere_->SetTransform(t);
				contactSphere_->GetMartial()->SetColor( { scmd.color[0], scmd.color[1], scmd.color[2], scmd.color[3] } );
				contactSphere_->SettingWvp(camera_->GetViewMatrix());
				Draw::DrawSphere(contactSphere_.get());
			} else if (scmd.type == HapiColi::SolidRenderCommand::Box) {
				Transform t;
				t.scale = {scmd.scale.x, scmd.scale.y, scmd.scale.z};
				t.rotate = {scmd.rotation.x, scmd.rotation.y, scmd.rotation.z};
				t.translate = {scmd.position.x, scmd.position.y, scmd.position.z};
				normalBox_->SetTransform(t);
				normalBox_->GetMartial()->SetColor( { scmd.color[0], scmd.color[1], scmd.color[2], scmd.color[3] } );
				normalBox_->SettingWvp(camera_->GetViewMatrix());
				Draw::DrawObj(normalBox_.get());
			}
		}
	} else {
		for (auto& lineObj : debugLines_) {
			LineVertexData vertices[2] = {
				{ {0,0,0}, {1,1,1,1} },
				{ {0,0,0}, {1,1,1,1} }
			};
			lineObj->SetVertex(vertices);
		}
	}
}