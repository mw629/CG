#include "GameScene.h"
#include "Graphics/Render/Draw.h"
#include <imgui.h>
#include <memory>
#include <System/CollisionManager.h>
#include <Engine.h>
#include "../../Editer/EditorManager.h"
#include "AssetManager.h"
#include <GameObjects/Object/3d/Model.h>
#include <algorithm>
#include <cmath>
#include <Math/Calculation.h>

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
#ifdef _USE_IMGUI
	ImGui::Begin("GameScene");

	camera_.get()->ImGui();

	if (ImGui::CollapsingHeader("GameScene Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool cameraChanged = false;
		if (ImGui::DragFloat3("Translate", &cameraTransform_.translate.x, 0.1f)) cameraChanged = true;
		if (ImGui::DragFloat3("Rotate", &cameraTransform_.rotate.x, 0.01f)) cameraChanged = true;
		if (ImGui::DragFloat3("Scale", &cameraTransform_.scale.x, 0.01f)) cameraChanged = true;

		if (cameraChanged) {
			camera_->SetTransform(cameraTransform_);
			gameCamera_->SetTransform(cameraTransform_);
		}

		ImGui::Separator();
		ImGui::Text("Presets:");
		if (ImGui::Button("Behind View")) {
			SetCameraToBehind();
		}
		ImGui::SameLine();
		if (ImGui::Button("Left Side View")) {
			cameraTransform_.scale = { 1.0f, 1.0f, 1.0f };
			cameraTransform_.rotate = { 0.3f, 1.0472f, 0.0f };
			cameraTransform_.translate = { -20.0f, 8.0f, -5.0f }; 
			camera_->SetTransform(cameraTransform_);
			gameCamera_->SetTransform(cameraTransform_);
		}
		ImGui::SameLine();
		if (ImGui::Button("Right Side View")) {
			SetCameraToRightSide();
		}
	}

	if (ImGui::CollapsingHeader("Game State", ImGuiTreeNodeFlags_DefaultOpen)) {
		const char* stateNames[] = { "Playing", "Paused", "PlayerHit", "GameClear", "GameOver", "Editor" };
		ImGui::Text("Game State: %s", stateNames[gameState_]);
		ImGui::Separator();

		// プレイヤー情報
		if (ImGui::TreeNode("Player Info")) {
			const Transform& playerTransform = player_->GetTransform();
			ImGui::Text("Position: (%.2f, %.2f, %.2f)", 
				playerTransform.translate.x, 
				playerTransform.translate.y, 
				playerTransform.translate.z);
			ImGui::Text("Rolling: %s", player_->GetIsRolling() ? "YES" : "NO");
			ImGui::TreePop();
		}

		// ステージ情報
		if (ImGui::TreeNode("Stage Info")) {
			ImGui::Text("Scroll Speed: %.3f", stageSettings_->GetScrollSpeed());
			ImGui::Text("Base Scroll Speed: %.3f", stageSettings_->GetBaseScrollSpeed());
			ImGui::Text("Max Scroll Speed: %.3f", stageSettings_->GetMaxScrollSpeed());
			ImGui::Text("Lane Index: Min=%d, Max=%d", 
				stageSettings_->GetMinLaneIndex(), 
				stageSettings_->GetMaxLaneIndex());
			ImGui::Text("Lane Width: %.2f", stageSettings_->GetLaneWidth());
			ImGui::TreePop();
		}

		ImGui::Separator();

		// スコアとランキング
		ImGui::Text("Current Distance: %.2f m", currentDistance_);
		if (ImGui::TreeNode("Distance Top 3 Ranking")) {
			for (int i = 0; i < 3; i++) {
				if (topRankings_[i] > 0.0f) {
					ImGui::Text("Rank %d: %.2f m", i + 1, topRankings_[i]);
				} else {
					ImGui::Text("Rank %d: ---", i + 1);
				}
			}
			ImGui::TreePop();
		}

		ImGui::Text("Current Score: %.0f", currentScore_);
		if (ImGui::TreeNode("Score Top 3 Ranking")) {
			for (int i = 0; i < 3; i++) {
				if (topScoreRankings_[i] > 0.0f) {
					ImGui::Text("Rank %d: %.0f", i + 1, topScoreRankings_[i]);
				} else {
					ImGui::Text("Rank %d: ---", i + 1);
				}
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::Button("Reset Game", ImVec2(120, 0))) {
			gameState_ = GameState::Playing;
			camera_->SetDebugCamera(false);
			stageSettings_->Reset();
			SetCameraToBehind();
			player_->Reset();
			currentDistance_ = 0.0f;
			currentScore_ = 0.0f;
			bonusEnemyHitCount_ = 0;
			isRightSideMode_ = false;
			rightSideDistance_ = 0.0f;
		}
	}

	// ステージ設定のデバッグパネル
	if (ImGui::CollapsingHeader("Stage Settings Debug")) {
		int laneCount = stageSettings_->GetLaneCount();
		if (ImGui::SliderInt("Lane Count", &laneCount, 1, 11)) {
			// Ensure it's preferably an odd number, or just pass it to the setter
			stageSettings_->SetLaneCount(laneCount);
		}

		float laneWidth = stageSettings_->GetLaneWidth();
		if (ImGui::SliderFloat("Lane Width", &laneWidth, 1.0f, 10.0f)) {
			stageSettings_->SetLaneWidth(laneWidth);
		}

		float baseSpeed = stageSettings_->GetBaseScrollSpeed();
		if (ImGui::SliderFloat("Base Scroll Speed", &baseSpeed, 0.0f, 1.0f)) {
			stageSettings_->SetBaseScrollSpeed(baseSpeed);
		}

		float maxSpeed = stageSettings_->GetMaxScrollSpeed();
		if (ImGui::SliderFloat("Max Scroll Speed", &maxSpeed, baseSpeed, 2.0f)) {
			stageSettings_->SetMaxScrollSpeed(maxSpeed);
		}

		float accel = stageSettings_->GetScrollAcceleration();
		if (ImGui::SliderFloat("Scroll Acceleration", &accel, 0.0f, 0.01f)) {
			stageSettings_->SetScrollAcceleration(accel);
		}

		float interval = stageSettings_->GetObstacleInterval();
		if (ImGui::SliderFloat("Obstacle Interval", &interval, 5.0f, 50.0f)) {
			stageSettings_->SetObstacleInterval(interval);
		}
	}

	particleManager_->ImGui();

	if (ImGui::CollapsingHeader("Post Effect Manager (All 16 Shaders)", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Auto Dynamic State Mode", &autoPostEffectEnabled_);
		ImGui::SameLine();
		ImGui::Checkbox("Retro Pixel Mode Trigger", &enableRetroPixelMode_);

		if (PostEffect::s_instances.size() >= 1) {
			ImGui::Text("Pass 0 (Environment Base): %s", PostEffect::s_instances[0]->GetActiveShaderName().c_str());
		}
		if (PostEffect::s_instances.size() >= 2) {
			ImGui::Text("Pass 1 (Dynamic Overlay): %s", PostEffect::s_instances[1]->GetActiveShaderName().c_str());
		}

		if (autoPostEffectEnabled_) {
			ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.3f, 1.0f), "Auto-triggering all 16 shaders based on game events!");
			ImGui::Separator();
			ImGui::Text("Event Simulators:");
			if (ImGui::Button("Simulate Hit (Glitch)")) {
				gameState_ = GameState::PlayerHit;
				hitGlitchTimer_ = 0.0f;
			}
			ImGui::SameLine();
			if (ImGui::Button("Simulate Game Over (Dissolve->Gray)")) {
				gameState_ = GameState::GameOver;
				dissolveTimer_ = 0.0f;
			}
			ImGui::SameLine();
			if (ImGui::Button("Simulate Clear (Sepia)")) {
				gameState_ = GameState::GameClear;
			}
		} else {
			ImGui::TextColored(ImVec4(0.9f, 0.6f, 0.2f, 1.0f), "Manual Override Mode (Select any of 16 shaders below):");
			if (!PostEffect::s_registeredEffects.empty()) {
				std::vector<const char*> names;
				for (const auto& e : PostEffect::s_registeredEffects) {
					names.push_back(e.first.c_str());
				}
				ImGui::Combo("Pass 0 Shader", &manualPass0Index_, names.data(), (int)names.size());
				ImGui::Combo("Pass 1 Shader", &manualPass1Index_, names.data(), (int)names.size());
			}
		}

		ImGui::Separator();
		if (ImGui::TreeNode("Pass 0 Inspector & Fine-Tuning")) {
			if (PostEffect::s_instances.size() >= 1) {
				PostEffect::s_instances[0]->ImGuiWindow();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Pass 1 Inspector & Fine-Tuning")) {
			if (PostEffect::s_instances.size() >= 2) {
				PostEffect::s_instances[1]->ImGuiWindow();
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Light Settings & Bloom Control", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (mainLight_) {
			ImGui::Text("Main Directional Light:");
			ImGui::ColorEdit4("Dir Color", &mainLight_->color_.x);
			ImGui::DragFloat("Dir Intensity", &mainLight_->intensity_, 0.05f, 0.0f, 10.0f);
		}
		if (playerPointLight_) {
			ImGui::Separator();
			ImGui::Text("Player Point Light:");
			ImGui::ColorEdit4("Point Color", &playerPointLight_->color_.x);
			ImGui::DragFloat("Point Intensity", &playerPointLight_->intensity_, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("Point Radius", &playerPointLight_->radius_, 0.5f, 1.0f, 50.0f);
		}
		if (stageSpotLight_) {
			ImGui::Separator();
			ImGui::Text("Stage Spot Light:");
			ImGui::ColorEdit4("Spot Color", &stageSpotLight_->color_.x);
			ImGui::DragFloat("Spot Intensity", &stageSpotLight_->intensity_, 0.1f, 0.0f, 20.0f);
			ImGui::DragFloat("Spot Distance", &stageSpotLight_->distance_, 0.5f, 1.0f, 100.0f);
		}
	}

	ImGui::End();

	Matrix4x4 projection = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
	editorUI_->Draw(gameObjectManager_.get(), view, projection);

	// Stopモードの時だけギズモ描画コールバックをSceneウィンドウに登録する
#ifdef _USE_IMGUI
	if (!EditorManager::IsPlaying()) {
		EditorManager::SetSceneOverlayCallback([this]() {
			Matrix4x4 proj = MakePerspectiveFovMatrix(0.45f, float(1280.0f) / float(720.0f), 0.1f, 100.0f);
			editorUI_->DrawGizmoInScene(view, proj);
		});
	} else {
		EditorManager::ClearSceneOverlayCallback();
	}
#endif

	// ポーズメニュー
	if (gameState_ == GameState::Paused) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("PAUSED");
		ImGui::Separator();
		if (ImGui::Button("Resume (ESC)", ImVec2(200, 40))) {
			gameState_ = GameState::Playing;
		}
		if (ImGui::Button("Return to Title", ImVec2(200, 40))) {
			nextSceneID_ = SceneID::Title;
			sceneChangeRequest_ = true;
		}
		ImGui::End();
	}

	// ゲームオーバーメニュー
	if (gameState_ == GameState::GameOver) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::Begin("Game Over Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("GAME OVER");
		ImGui::Separator();

		ImGui::Text("Your Distance: %.2f m", currentDistance_);
		ImGui::Text("Your Score: %.0f", currentScore_);
		ImGui::Separator();
		ImGui::Text("--- DISTANCE TOP 3 RANKING ---");
		for (int i = 0; i < 3; i++) {
			if (topRankings_[i] > 0.0f) {
				ImGui::Text("  %d. %.2f m", i + 1, topRankings_[i]);
			} else {
				ImGui::Text("  %d. ---", i + 1);
			}
		}
		ImGui::Separator();
		ImGui::Text("--- SCORE TOP 3 RANKING ---");
		for (int i = 0; i < 3; i++) {
			if (topScoreRankings_[i] > 0.0f) {
				ImGui::Text("  %d. %.0f", i + 1, topScoreRankings_[i]);
			} else {
				ImGui::Text("  %d. ---", i + 1);
			}
		}
		ImGui::Separator();

		if (ImGui::Button("Restart (1)", ImVec2(200, 40))) {
			gameState_ = GameState::Playing;
			stageSettings_->Reset();
			SetCameraToBehind();
			// PostEffect::SetActivePostEffect(PostEffect::Type::Normal);
			player_->Reset();
			currentDistance_ = 0.0f;
			currentScore_ = 0.0f;
			bonusEnemyHitCount_ = 0;
			isRightSideMode_ = false;
			rightSideDistance_ = 0.0f;
		}
		if (ImGui::Button("Return to Title (2)", ImVec2(200, 40))) {
			nextSceneID_ = SceneID::Title;
			sceneChangeRequest_ = true;
		}
		ImGui::End();
	}

#endif // _USE_IMGUI
}



void GameScene::Initialize() {

	sceneID_ = SceneID::Game;
	
	if (!EditorManager::IsPlaying()) {
		gameState_ = GameState::Editor;
		camera_->SetDebugCamera(true);
	} else {
		gameState_ = GameState::Playing;
		camera_->SetDebugCamera(false);
	}
	sceneChangeRequest_ = false;

	// パーティクルマネージャーの初期化
	particleManager_->Initialize();

	// コリジョンマネージャーの初期化
	collisionManager_ = std::make_unique<CollisionManager>();

	// camera_->SetDebugCamera() は上記で設定済み
	camera_->SetTransform(cameraTransform_);
	camera_->Update();

	gameCamera_->SetDebugCamera(false); // Game Cameraは常にDebug操作を受け付けない
	gameCamera_->SetTransform(cameraTransform_);
	gameCamera_->Update();

	// Game View描画コールバックの登録
	EditorManager::SetGameViewDrawCallback([this](class Draw& draw) {
		Matrix4x4 gameViewMat = gameCamera_->GetViewMatrix();

		// 一時的にSkyBoxをGameCameraの位置へ移動
		Transform originalSkyBoxT = skyBox_->GetTransform();
		Transform gameSkyBoxT = originalSkyBoxT;
		gameSkyBoxT.translate = gameCamera_->GetTransform().translate;
		skyBox_->SetTransform(gameSkyBoxT);

		// ゲームカメラのView行列でオブジェクトのWVPを更新 (speedMultiplier=0.0f でアニメーションは進めない)
		ObjectBase::SetWvpIndex(1);
		EffectDefinition::SetWvpIndex(1);

		gameObjectManager_->UpdateAll(gameViewMat, 0.0f);
		stageSettings_->EditorUpdate(gameViewMat);
		particleManager_->EditorUpdate(gameViewMat);

		draw.SetCamera(gameCamera_.get());
		draw.SetEnvironmentTexture(skyBoxTexture_);
		gameObjectManager_->DrawAll(draw);
		stageSettings_->Draw(draw);
		particleManager_->Draw(draw);

		// SkyBoxの位置を元に戻す
		skyBox_->SetTransform(originalSkyBoxT);

		ObjectBase::SetWvpIndex(0);
		EffectDefinition::SetWvpIndex(0);
	});

	// スカイボックスの初期化
	skyBoxTexture_ = texture_.get()->CreateTexture("Resources/DDS/SnowWorld.dds");
	skyBox_.get()->Initialize(skyBoxTexture_);
	skyBox_.get()->SetShader("SkyBoxShader");
	skyBox_.get()->SetLighting(false);
	skyBox_.get()->SetTransform(skyBoxTransform_);
	skyBox_.get()->name_ = "SkyBox";

	// プレイヤーの初期化
	ModelData modelData = AssimpLoadObjFile("Resources/Model/Player", "Player.obj");
	player_->Initialize(modelData);

	// オブジェクトマネージャーへの登録
	gameObjectManager_->Clear();
	auto skyboxRenderObj = std::make_shared<RenderObject>(skyBox_);
	skyboxRenderObj->SetName("SkyBox");
	gameObjectManager_->AddObject(skyboxRenderObj);
	player_->SetName("Player");
	gameObjectManager_->AddObject(player_);

	// ライティングの設置（DirectionalLight, PointLight, SpotLight）
	mainLight_ = std::make_shared<DirectionalLight>();
	mainLight_->SetName("MainDirectionalLight");
	mainLight_->color_ = { 1.3f, 1.25f, 1.15f, 1.0f }; // 温かい強力な主太陽光
	mainLight_->intensity_ = 1.5f;
	Transform mainLightT;
	mainLightT.scale = { 1.0f, 1.0f, 1.0f };
	mainLightT.rotate = { 0.75f, -0.6f, 0.0f }; // 斜め上からの光で強烈なハイライトと陰影を付与
	mainLightT.translate = { 0.0f, 25.0f, -10.0f };
	mainLight_->SetTransform(mainLightT);
	gameObjectManager_->AddObject(mainLight_);

	playerPointLight_ = std::make_shared<PointLight>();
	playerPointLight_->SetName("PlayerPointLight");
	playerPointLight_->color_ = { 1.0f, 0.85f, 0.4f, 1.0f }; // プレイヤー追従ゴールドポイントライト
	playerPointLight_->intensity_ = 4.0f;
	playerPointLight_->radius_ = 20.0f;
	playerPointLight_->decay_ = 1.2f;
	Transform playerLightT;
	playerLightT.scale = { 1.0f, 1.0f, 1.0f };
	playerLightT.translate = { 0.0f, 3.0f, 0.0f };
	playerPointLight_->SetTransform(playerLightT);
	gameObjectManager_->AddObject(playerPointLight_);

	stageSpotLight_ = std::make_shared<SpotLight>();
	stageSpotLight_->SetName("StageSpotLight");
	stageSpotLight_->color_ = { 0.8f, 0.95f, 1.0f, 1.0f }; // スポットライト
	stageSpotLight_->intensity_ = 3.5f;
	stageSpotLight_->distance_ = 50.0f;
	stageSpotLight_->decay_ = 1.2f;
	stageSpotLight_->cosAngle_ = std::cos(0.5f);
	stageSpotLight_->cosFalloffStart_ = std::cos(0.3f);
	Transform spotT;
	spotT.scale = { 1.0f, 1.0f, 1.0f };
	spotT.rotate = { 0.65f, 0.0f, 0.0f };
	spotT.translate = { 0.0f, 20.0f, -8.0f };
	stageSpotLight_->SetTransform(spotT);
	gameObjectManager_->AddObject(stageSpotLight_);

	// エディターでの保存・読み込み先をJsonSceneに設定
	EditorManager::SetSaveCallback([this](const std::string& filePath) {
		gameObjectManager_->SaveScene(filePath);
	});
	EditorManager::SetLoadCallback([this](const std::string& filePath) {
		gameObjectManager_->LoadScene(filePath);
	});
	EditorManager::SetFileDropCallback([this](const std::string& dropPath) {
		size_t lastSlash = dropPath.find_last_of("/\\");
		if (lastSlash != std::string::npos) {
			std::string dirPath = dropPath.substr(0, lastSlash);
			std::string fileName = dropPath.substr(lastSlash + 1);
			
			size_t extPos = fileName.find_last_of(".");
			if (extPos != std::string::npos) {
				std::string ext = fileName.substr(extPos);
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
				if (ext == ".obj" || ext == ".gltf") {
					try {
						ModelData modelData = AssetManager::LoadModel(dirPath, fileName);
						auto model = std::make_shared<Model>();
						model->Initialize(modelData);
						model->name_ = fileName;
						
						auto renderObj = std::make_shared<RenderObject>(model);
						renderObj->SetName(fileName);
						gameObjectManager_->AddObject(renderObj);
					} catch (const std::exception& e) {
						// Error handling if loading fails
					}
				}
			}
		}
	});

	// ステージの初期化
	ModelData roadModelData = AssetManager::LoadModel("Resources/Block", "Block.obj");
	ModelData obstacleModelData = AssetManager::LoadModel("Resources/Block", "Block.obj");
	stageSettings_->Initialize(roadModelData, obstacleModelData, modelData, gameObjectManager_.get());

	// 指定したJsonファイルを初期シーンとして読み込む
	gameObjectManager_->LoadScene(initialSceneJson_);

	currentDistance_ = 0.0f;
	currentScore_ = 0.0f;
	bonusEnemyHitCount_ = 0;
	isRightSideMode_ = false;
	rightSideDistance_ = 0.0f;
}

void GameScene::Update() {
	ObjectBase::SetWvpIndex(0);
	EffectDefinition::SetWvpIndex(0);

#ifdef DEBUG
	if (Input::PushKey(DIK_Q)) {
		SetCameraToBehind();
	}
	if (Input::PushKey(DIK_E)) {
		SetCameraToRightSide();
	}
#endif // DEBUG

	

	// PostEffect::SetActivePostEffect(PostEffect::Type::GaussianFilter);

	// Engine側のPlay/Stop状態に同期してゲームステートを切り替え
	bool isEnginePlaying = EditorManager::IsPlaying();
	if (isEnginePlaying && gameState_ == GameState::Editor) {
		gameState_ = GameState::Playing;
		camera_->SetDebugCamera(false);
	} else if (!isEnginePlaying && gameState_ == GameState::Playing) {
		gameState_ = GameState::Editor;
		camera_->SetDebugCamera(true);
	}

	UpdateCameraTransition();

	camera_->Update();
	view = camera_->GetViewMatrix();

	gameCamera_->Update();

	// SkyBoxをカメラの位置に追従させる（無限遠の背景として機能させるため）
	Transform skyboxTransform = skyBox_->GetTransform();
	skyboxTransform.translate = camera_->GetTransform().translate;
	skyBox_->SetTransform(skyboxTransform);
	
	// skyBox_ is now updated in gameObjectManager_

	if (gameState_ == GameState::Playing) {
		PlayingUpdate();

		if(Input::PushKey(DIK_ESCAPE)){
			gameState_ = GameState::Paused;
		}
	}
	else if (gameState_ == GameState::Paused) {
		PausedUpdate();
		if (Input::PushKey(DIK_ESCAPE)) {
			gameState_ = GameState::Playing;
		}
	}
	else if (gameState_ == GameState::PlayerHit) {
		PlayerHitUpdate();
	}
	else if(gameState_ == GameState::GameOver){
		// 1でリスタート
		if (Input::PushKey(DIK_1)) {
			gameState_ = GameState::Playing;
			stageSettings_->Reset();
			SetCameraToBehind();
			// PostEffect::SetActivePostEffect(PostEffect::Type::Normal);
			player_->Reset();
			particleManager_->ClearHitParticles(); // 前回の煙をリセット
			currentDistance_ = 0.0f;
			currentScore_ = 0.0f;
			bonusEnemyHitCount_ = 0;
			isRightSideMode_ = false;
			rightSideDistance_ = 0.0f;
		}
		// 2でタイトルへ
		if (Input::PushKey(DIK_2)) {
			nextSceneID_ = SceneID::Title;
			sceneChangeRequest_ = true;
		}
	}
	else if (gameState_ == GameState::Editor) {
		EditorUpdate();
	}

	UpdatePostEffects();
}

void GameScene::UpdatePostEffects() {
	if (PostEffect::s_instances.empty()) return;

	PostEffect* pass0 = PostEffect::s_instances[0]; // Environment / Base Pass
	PostEffect* pass1 = (PostEffect::s_instances.size() >= 2) ? PostEffect::s_instances[1] : nullptr; // Dynamic State / Event Pass

	if (!autoPostEffectEnabled_) {
		// 手動指定モード
		if (!PostEffect::s_registeredEffects.empty()) {
			if (manualPass0Index_ >= 0 && manualPass0Index_ < (int)PostEffect::s_registeredEffects.size()) {
				pass0->SetActivePostEffect(PostEffect::s_registeredEffects[manualPass0Index_].second);
			}
			if (pass1 && manualPass1Index_ >= 0 && manualPass1Index_ < (int)PostEffect::s_registeredEffects.size()) {
				pass1->SetActivePostEffect(PostEffect::s_registeredEffects[manualPass1Index_].second);
			}
		}
		return;
	}

	// 自動状態連動モード (全16種類のエフェクトを状況に応じてダイナミック切り替え)
	
	// Pass 0: 3Dステージ全体の環境レイヤー (FogShader)
	pass0->SetActivePostEffect("FogShader");
	pass0->SetRatio(0.5f);
	pass0->SetValue1(0.05f); // 密度
	pass0->SetValue2(8.0f);  // 開始距離
	pass0->SetColor(0.7f, 0.8f, 0.95f);

	if (!pass1) return;

	// Pass 1: ゲームステート・イベントに応じた動的オーバーレイエフェクト
	switch (gameState_) {
	case GameState::Editor:
		// 1. Editorモード: LuminanceOutLineShader (CAD/図面風・アニメ調エッジ)
		pass1->SetActivePostEffect("LuminanceOutLineShader");
		break;

	case GameState::Paused:
		// 2. Paused状態: GaussianFilterShader (高品質ガウスぼかし)
		pass1->SetActivePostEffect("GaussianFilterShader");
		pass1->SetBlurStrength(3.5f);
		break;

	case GameState::PlayerHit:
		// 3. PlayerHit状態: RandomShader (ノイズ・グリッチ衝突インパクト)
		hitGlitchTimer_ += 1.0f / 60.0f;
		pass1->SetActivePostEffect("RandomShader");
		pass1->SetValue1(0.7f); // ノイズ強度
		pass1->SetRatio(1.0f);
		break;

	case GameState::GameOver:
		// 4 & 5. GameOver移行期: DissolveShader → GrayScaleShader (分解 → 彩度喪失モノクロ)
		dissolveTimer_ += 1.0f / 60.0f;
		if (dissolveTimer_ < 1.2f) {
			pass1->SetActivePostEffect("DissolveShader");
			pass1->SetTime(dissolveTimer_);
		} else {
			pass1->SetActivePostEffect("GrayScaleShader");
		}
		break;

	case GameState::GameClear:
		// 6. GameClear状態: GrayScaleSepiaToneShader (黄金色のノスタルジックセピア)
		pass1->SetActivePostEffect("GrayScaleSepiaToneShader");
		break;

	case GameState::Playing:
	default:
		// プレイ中の状況判定
		hitGlitchTimer_ = 0.0f;
		dissolveTimer_ = 0.0f;

		if (isCameraTransitioning_) {
			// 7. カメラ視点移動中: SmoothingShader (フォーカスソフトブラー)
			pass1->SetActivePostEffect("SmoothingShader");
			pass1->SetBlurStrength(2.0f);
		}
		else if (isRightSideMode_) {
			// 8. 右サイドカメラモード (CameraItem取得): OutLineShader (3D深度輪郭強調)
			pass1->SetActivePostEffect("OutLineShader");
		}
		else if (player_->GetIsRolling()) {
			// 9. プレイヤーローリング中: RadialBlurShader (超高速放射状ブラー)
			pass1->SetActivePostEffect("RadialBlurShader");
			pass1->SetBlurStrength(3.0f);
		}
		else if (stageSettings_->GetScrollSpeed() >= stageSettings_->GetMaxScrollSpeed() * 0.85f) {
			// 10. 最高速ブースト到達時: BloomShader (輝度発光・エネルギーブースト)
			pass1->SetActivePostEffect("BloomShader");
			pass1->SetValue1(0.7f); // Luminance threshold
			pass1->SetValue2(1.5f); // Bloom intensity
		}
		else if (bonusEnemyHitCount_ > 0 && (bonusEnemyHitCount_ % 3 == 0)) {
			// 11. ボーナスエネミー連続Hit中 (Combo Spree): HalftoneDotShader (コミック風ハーフトーン網点)
			pass1->SetActivePostEffect("HalftoneDotShader");
			pass1->SetValue1(12.0f); // 格子サイズ
			pass1->SetValue2(0.5f);  // ボケ足
			pass1->SetRatio(0.6f);   // 適用度
		}
		else if (player_->GetHasBarrier() || enableRetroPixelMode_) {
			// 12. バリア獲得 / レトロモード: PixelateShader (モザイクピクセルアート)
			pass1->SetActivePostEffect("PixelateShader");
			pass1->SetUsePixelation(true);
			pass1->SetBlockCountX(160.0f);
			pass1->SetBlockCountY(90.0f);
		}
		else if (currentScore_ >= 800.0f) {
			// 13. マイルストーンハイスコア到達: PosterizationShader (色階調ポスタリゼーション)
			pass1->SetActivePostEffect("PosterizationShader");
			pass1->SetValue1(5.0f); // 色階調ステップ数
		}
		else {
			// 14 & 15. 通常走航時: VignettingShader (四隅ヴィネット) または CopyShader (ノーマル)
			pass1->SetActivePostEffect("VignettingShader");
			pass1->SetValue1(0.85f);
			pass1->SetValue2(0.35f);
		}
		break;
	}
}

void GameScene::Draw(class Draw& draw) {
	//カメラの設定
	draw.SetCamera(camera_.get());
	//背景の設定
	draw.SetEnvironmentTexture(skyBoxTexture_);

	// オブジェクトの一括描画（SkyBox, Player など）
	gameObjectManager_->DrawAll(draw);

	// ステージ描画（道路 + 障害物）
	stageSettings_->Draw(draw);

	// ポーズ中の描画
	if (gameState_ == GameState::Paused) {
		pauseSystem_->Draw(draw);
	}

	// ヒットエフェクトの描画
	particleManager_->Draw(draw);
}

void GameScene::PlayerHitUpdate()
{
	// カメラやビューの更新は GameScene::Update で行われている
	particleManager_->PlayerHitUpdate(view);

	// プレイヤーのノックバックアニメーションを更新
	// (GameScene側の全体更新は停止し、プレイヤーのみ更新)
	player_->Update(view, 1.0f);

	if (player_->IsHitAnimationFinished()) {
		gameState_ = GameState::GameOver;
	}
}

void GameScene::PlayingUpdate()
{
	float timeScale = EditorManager::GetPlaySpeed();
	float speedMultiplier = 1.0f;
	if (stageSettings_->GetBaseScrollSpeed() > 0.0f) {
		speedMultiplier = stageSettings_->GetScrollSpeed() / stageSettings_->GetBaseScrollSpeed();
	}

	currentDistance_ += stageSettings_->GetScrollSpeed() * timeScale;
	currentScore_ = currentDistance_ + (bonusEnemyHitCount_ * 200.0f * stageSettings_->GetScrollSpeed());

	CheckKeepRolling();
	
	// 右サイドモード（カメラアイテム取得後）の更新
	if (isRightSideMode_) {
		rightSideDistance_ += stageSettings_->GetScrollSpeed() * timeScale;
		if (rightSideDistance_ >= 200.0f) {
			SetCameraToBehind();
			isRightSideMode_ = false;
			rightSideDistance_ = 0.0f;
		}
	}

	// プレイヤー追従ライトの位置・強度更新 (ライティングとBloomの相乗効果演出)
	if (playerPointLight_ && player_) {
		const Transform& pTransform = player_->GetTransform();
		Transform lightT = playerPointLight_->GetTransform();
		lightT.translate = { pTransform.translate.x, pTransform.translate.y + 2.5f, pTransform.translate.z + 1.0f };
		playerPointLight_->SetTransform(lightT);

		if (player_->GetIsRolling()) {
			playerPointLight_->color_ = { 0.3f, 0.9f, 1.0f, 1.0f }; // ローリング時シアン光
			playerPointLight_->intensity_ = 5.5f;
		} else if (stageSettings_->GetScrollSpeed() >= stageSettings_->GetMaxScrollSpeed() * 0.85f) {
			playerPointLight_->color_ = { 1.0f, 0.95f, 0.7f, 1.0f }; // 最高速ゴールドブースト光
			playerPointLight_->intensity_ = 6.0f;
		} else {
			playerPointLight_->color_ = { 1.0f, 0.85f, 0.4f, 1.0f };
			playerPointLight_->intensity_ = 4.0f;
		}
	}

	// Update Player lane constraints
	player_->SetLaneLimits(stageSettings_->GetMinLaneIndex(), stageSettings_->GetMaxLaneIndex(), stageSettings_->GetLaneWidth());

	// オブジェクトの一括更新
	gameObjectManager_->UpdateAll(view, speedMultiplier * timeScale);
	
	stageSettings_->Update(view, timeScale);

	// 以前の当たり判定チェック
	CheckCollisions();

	// Componentベースの当たり判定チェック
	collisionManager_->UpdateCollisions(gameObjectManager_.get());

	particleManager_->PlayingUpdate(view, player_->GetTransform().translate);
	particleManager_->UpdateBonusEffectEmit(timeScale, player_->GetTransform().translate);

	// 走っている間（転がっていなくて地面にいる時）	// プレイヤーの足元に砂埃エフェクトを生成
	if (!player_->GetIsRolling() && player_->GetTransform().translate.y <= 3.01f) {
		particleManager_->EmitDust(player_->GetTransform().translate);
	}
}

void GameScene::PausedUpdate()
{
	pauseSystem_->Update();
}

void GameScene::EditorUpdate()
{
	// Editor mode doesn't progress the game scroll or obstacle positions.
	// But we still want to update objects (like their transforms).
	gameObjectManager_->UpdateAll(view, 0.0f);
	stageSettings_->EditorUpdate(view);

	// パーティクルがデバッグカメラに対応するように、EditorUpdate() を呼び出す
	particleManager_->EditorUpdate(view);
}

void GameScene::CheckCollisions()
{
	// プレイヤーのAABBを生成
	const Transform& playerTransform = player_->GetTransform();
	float playerHeight = player_->GetIsRolling() ? 0.5f : 1.5f; // 転がり中は低くなる
	AABB playerAABB = Collision::MakeAABB(playerTransform, 0.8f, playerHeight, 0.8f);

	// 全障害物との当たり判定
	for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
		Obstacle* obstacle = stageSettings_->GetObstacle(i);
		if (!obstacle->GetIsActive() || obstacle->GetIsHit()) continue;

		AABB obstacleAABB = Collision::MakeAABB(
			obstacle->GetTransform(),
			obstacle->GetCollisionWidth(),
			obstacle->GetCollisionHeight(),
			obstacle->GetCollisionDepth()
		);

		if (Collision::CheckAABB(playerAABB, obstacleAABB)) {
			// 誘導床（GuideFloor）の判定
			if (obstacle->GetType() == Obstacle::Type::GuideFloor) {
				// プレイヤーを滑らかに中央へ誘導 (30フレーム)
				player_->StartForceToCenter(30.0f);
				
				// 【演出ポイント: パーティクル】
				// 加速や誘導を示すスピード線のエフェクトや、足元の衝撃波を出す
				// particleManager_->EmitGuideEffect(player_->GetTransform().translate);
				
				// 【演出ポイント: サウンド】
				// SoundManager::Play("GuideDash_SE"); // シューッというSE等
				
				continue; // ゲームオーバーにはならない
			}

			if (obstacle->GetType() == Obstacle::Type::Bonus) {
				// ボーナスエネミーに当たった場合の処理（吹き飛ばす）
				obstacle->OnBlowAway();
				bonusEnemyHitCount_++; // スコア（距離）ボーナス

				// プレイヤーの足元にRingエフェクトを出す
				particleManager_->EmitShockwave(player_->GetTransform().translate);

				// プレイヤーの足元にCylinderエフェクトを出す
				particleManager_->StartBonusEffect(120.0f); // 60FPS環境で2秒間
				particleManager_->EmitBonusCylinder(player_->GetTransform().translate);

				continue; // ゲームオーバーにはならず、次の判定へ
			}

			if (obstacle->GetType() == Obstacle::Type::CameraItem) {
				obstacle->OnHit();
				// カメラ右移動の呼び出し
				SetCameraToRightSide();
				isRightSideMode_ = true;
				rightSideDistance_ = 0.0f;

				// プレイヤーの足元にRingエフェクトを出す(ボーナスと同様の演出)
				particleManager_->EmitShockwave(player_->GetTransform().translate);

				continue; // ゲームオーバーにはならず、次の判定へ
			}

			if (obstacle->GetType() == Obstacle::Type::BarrierItem) {
				obstacle->OnHit();
				player_->SetHasBarrier(true);
				particleManager_->EmitShockwave(player_->GetTransform().translate);
				continue;
			}

			if (obstacle->GetType() == Obstacle::Type::ClearItem) {
				obstacle->OnHit();
				particleManager_->EmitShockwave(player_->GetTransform().translate);
				
				// 画面内の障害物を吹き飛ばす
				for (int j = 0; j < stageSettings_->GetMaxObstacles(); j++) {
					Obstacle* obs = stageSettings_->GetObstacle(j);
					if (obs->GetIsActive() && 
						(obs->GetType() == Obstacle::Type::Low || 
						 obs->GetType() == Obstacle::Type::High || 
						 obs->GetType() == Obstacle::Type::Wall)) {
						obs->OnBlowAway(); 
					}
				}
				continue;
			}

			// プレイヤーがバリアを持っている場合は消費して防ぐ
			if (player_->GetHasBarrier()) {
				player_->SetHasBarrier(false);
				obstacle->OnBlowAway();
				particleManager_->EmitHitEffect(player_->GetTransform().translate);
				continue; // ゲームオーバーにならず次へ
			}

			// 衝突！ヒット演出へ移行
			gameState_ = GameState::PlayerHit;
			stageSettings_->SetGameOver(true);
			// PostEffect::SetActivePostEffect(PostEffect::Type::GrayScale);
			
			// プレイヤーのヒットアニメーション開始（Low障害物なら前へ転がる）
			bool isTrip = (obstacle->GetType() == Obstacle::Type::Low);
			player_->OnHit(isTrip);

			// エフェクトの発生位置をプレイヤーから取得する
			particleManager_->EmitHitEffect(player_->GetTransform().translate);

			// ランキング更新
			UpdateRanking();
			UpdateScoreRanking();
			
			break;
		}
	}
}

void GameScene::UpdateRanking()
{
	// 降順ソートでトップ3を保持
	for (int i = 0; i < 3; i++) {
		if (currentDistance_ > topRankings_[i]) {
			// シフト
			for (int j = 2; j > i; j--) {
				topRankings_[j] = topRankings_[j - 1];
			}
			topRankings_[i] = currentDistance_;
			break;
		}
	}
}

void GameScene::UpdateScoreRanking()
{
	// 降順ソートでトップ3を保持
	for (int i = 0; i < 3; i++) {
		if (currentScore_ > topScoreRankings_[i]) {
			// シフト
			for (int j = 2; j > i; j--) {
				topScoreRankings_[j] = topScoreRankings_[j - 1];
			}
			topScoreRankings_[i] = currentScore_;
			break;
		}
	}
}

void GameScene::CheckKeepRolling()
{
	bool keepRolling = false;
	if (player_->GetIsRolling()) {
		// Calculate a "standing up" AABB for the player
		const Transform& playerTransform = player_->GetTransform();
		AABB standingAABB = Collision::MakeAABB(playerTransform, 0.8f, 1.5f, 0.8f);

		for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
			Obstacle* obstacle = stageSettings_->GetObstacle(i);
			if (!obstacle->GetIsActive() || obstacle->GetIsHit()) continue;

			if (obstacle->GetType() == Obstacle::Type::High) {
				AABB obstacleAABB = Collision::MakeAABB(
					obstacle->GetTransform(),
					obstacle->GetCollisionWidth(),
					obstacle->GetCollisionHeight(),
					obstacle->GetCollisionDepth()
				);

				// もし立ち上がったら当たる位置にいるか？
				if (Collision::CheckAABB(standingAABB, obstacleAABB)) {
					// プレイヤーの中心が障害物の中心より奥（Z座標が大きい）なら
					if (playerTransform.translate.z > obstacle->GetTransform().translate.z) {
						keepRolling = true;
						break;
					}
				}
			}
		}
	}
	player_->SetKeepRolling(keepRolling);
}

void GameScene::SetCameraToRightSide() {
	Transform target;
	target.scale = { 1.0f, 1.0f, 1.0f };
	target.rotate = { 0.3f, -1.0472f, 0.0f };
	target.translate = { 30.0f, 15.0f, -5.0f };
	StartCameraTransition(target, 1);
}

void GameScene::SetCameraToBehind() {
	Transform target;
	target.scale = { 1.0f, 1.0f, 1.0f };
	target.rotate = { 0.3f, 0.0f, 0.0f };
	target.translate = { 0.0f, 8.0f, -15.0f };
	StartCameraTransition(target, 3);
}

void GameScene::StartCameraTransition(const Transform& targetTransform, int laneCount) {
	// 障害物の生成を即座に停止
	stageSettings_->SetSpawningPaused(true);

	// トランジションの予約を行う
	isCameraTransitionPending_ = true;
	pendingCameraTargetTransform_ = targetTransform;
	pendingLaneCount_ = laneCount;
}

void GameScene::UpdateCameraTransition() {
	if (isCameraTransitionPending_) {
		// アクティブな障害物が残っているかチェック
		bool hasActiveObstacles = false;
		for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
			Obstacle* obstacle = stageSettings_->GetObstacle(i);
			if (obstacle->GetIsActive()) {
				hasActiveObstacles = true;
				break;
			}
		}

		// 障害物が全て消えたら、実際のトランジションを開始する
		if (!hasActiveObstacles) {
			isCameraTransitionPending_ = false;
			isCameraTransitioning_ = true;
			cameraTransitionTimer_ = 0.0f;
			startCameraTransform_ = cameraTransform_;
			targetCameraTransform_ = pendingCameraTargetTransform_;
			stageSettings_->SetLaneCount(pendingLaneCount_);
		}
	}

	if (!isCameraTransitioning_) return;

	cameraTransitionTimer_ += 1.0f / 60.0f; // 毎フレームの時間を加算 (FPS固定なら)
	float t = cameraTransitionTimer_ / cameraTransitionDuration_;
	
	if (t >= 1.0f) {
		t = 1.0f;
		isCameraTransitioning_ = false;
		stageSettings_->SetSpawningPaused(false);
	}

	// easeInOut（スムーズな動きのため）
	float easeT = t * t * (3.0f - 2.0f * t);

	cameraTransform_.translate = Lerp(startCameraTransform_.translate, targetCameraTransform_.translate, easeT);
	
	// 弧を描くためのオフセット (Y軸方向に膨らむ)
	float arcHeight = 10.0f; 
	cameraTransform_.translate.y += std::sin(easeT * 3.14159265f) * arcHeight;

	cameraTransform_.rotate = Lerp(startCameraTransform_.rotate, targetCameraTransform_.rotate, easeT);
	cameraTransform_.scale = Lerp(startCameraTransform_.scale, targetCameraTransform_.scale, easeT);

	camera_->SetTransform(cameraTransform_);
	gameCamera_->SetTransform(cameraTransform_);
}
