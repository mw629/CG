#include "GameScene.h"
#include <imgui.h>
#include <memory>
#include <string>
#include "../../HapiColi/HapiColi.h"
#include "../../MatchaEngine/Math/Calculation.h"

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
#ifdef _USE_IMGUI
	ImGui::Begin("GameScene");

	camera_.get()->ImGui();

	if (ImGui::CollapsingHeader("Game State", ImGuiTreeNodeFlags_DefaultOpen)) {
		const char* stateNames[] = { "Playing", "Paused", "GameClear", "GameOver" };
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

		// ゲーム制御
		if (ImGui::Button("Reset Game", ImVec2(120, 0))) {
			gameState_ = GameState::Playing;
			stageSettings_->Reset();
			player_->Reset();
		}
	}

	// ステージ設定のデバッグパネル
	if (ImGui::CollapsingHeader("Stage Settings Debug")) {
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
	}

	ImGui::End();

	HapiColi::HapiColi::GetInstance().Update();

#endif // _USE_IMGUI
}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;
	gameState_ = GameState::Playing;
	sceneChangeRequest_ = false;



	camera_->SetDebugCamera(false); // ゲーム用カメラを使う
	camera_->SetTransform(cameraTransform_);
	camera_->Update();

	// プレイヤーの初期化
	ModelData modelData = AssimpLoadObjFile("resources/Model/Player", "player.obj");
	player_->Initialize(modelData);

	// ステージの初期化
	ModelData roadModelData = AssimpLoadObjFile("resources/Plane", "Plane.gltf");
	ModelData obstacleModelData = AssimpLoadObjFile("resources/Block", "Block.obj");
	stageSettings_->Initialize(roadModelData, obstacleModelData);

	// デバッグ用ラインはDraw時に必要に応じて動的に確保します

	HapiColi::HapiColi::GetInstance().Initialize();
}

void GameScene::Update() {
	HapiColi::HapiColi::GetInstance().BeginFrame(1.0f / 60.0f);


	camera_->Update();
	view = camera_->GetViewMatrix();

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
	else if(gameState_ == GameState::GameOver){
		// 1でリスタート
		if (Input::PushKey(DIK_1)) {
			gameState_ = GameState::Playing;
			stageSettings_->Reset();
			PostEffect::SetActivePostEffect(PostEffect::Type::Normal);
			player_->Reset();
		}
		// 2でタイトルへ
		if (Input::PushKey(DIK_2)) {
			nextSceneID_ = SceneID::Title;
			sceneChangeRequest_ = true;
		}
	}

	// デバッグ表示の切り替え (Cキー)
	if (Input::PushKey(DIK_C)) {
		isDrawColi_ = !isDrawColi_;
	}

	// 当たり判定のチェックと記録 (ポーズ中などどんな状態でも描画するために毎フレーム実行)
	CheckCollisions();

	HapiColi::HapiColi::GetInstance().EndFrame();
}

void GameScene::Draw() {
	//カメラの設定
	Draw::SetCamera(camera_.get());
	//背景の設定
	Draw::SetEnvironmentTexture(texture_.get()->CreateTexture("resources/DDS/rostock_laage_airport_4k.dds"));

	// ステージ描画（道路 + 障害物）
	stageSettings_->Draw();

	// プレイヤー描画
	player_->Draw();

	// ポーズ中の描画
	if (gameState_ == GameState::Paused) {
		pauseSystem_->Draw();
	}

	if (isDrawColi_) {
		HapiColi::HapiColi::GetInstance().BuildRenderCommands();
		const auto& renderCommands = HapiColi::HapiColi::GetInstance().GetRenderCommands();
		
		if (!renderCommands.empty()) {
			Draw::preDraw(LineShader, kBlendModeNormal);

			int lineIndex = 0;
			for (const auto& cmd : renderCommands) {
				// 足りなければLineを追加生成（サイズが変わった際やオブジェクトが増えた際に対応）
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
		}
	}

#ifdef _USE_IMGUI
#endif
}

void GameScene::PlayingUpdate()
{
	player_->Update(view);
	stageSettings_->Update(view);
}

void GameScene::PausedUpdate()
{
	pauseSystem_->Update();
}

void GameScene::CheckCollisions()
{
	// プレイヤーのAABBを生成
	const Transform& playerTransform = player_->GetTransform();
	float playerHeight = player_->GetIsRolling() ? 0.5f : 1.5f; // 転がり中は低くなる
	AABB playerAABB = Collision::MakeAABB(playerTransform, 0.8f, playerHeight, 0.8f);

	HapiColi::ObjectData playerData = HapiColi::ObjectData::CreateBox(
		"Player",
		{playerTransform.translate.x, playerTransform.translate.y, playerTransform.translate.z},
		{0.8f, playerHeight, 0.8f}
	);

	// 全障害物との当たり判定
	for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
		Obstacle* obstacle = stageSettings_->GetObstacle(i);
		if (!obstacle->GetIsActive()) continue;

		AABB obstacleAABB = Collision::MakeAABB(
			obstacle->GetTransform(),
			obstacle->GetCollisionWidth(),
			obstacle->GetCollisionHeight(),
			obstacle->GetCollisionDepth()
		);

		HapiColi::ObjectData obstacleData = HapiColi::ObjectData::CreateBox(
			"Obstacle_" + std::to_string(i),
			{obstacle->GetTransform().translate.x, obstacle->GetTransform().translate.y, obstacle->GetTransform().translate.z},
			{obstacle->GetCollisionWidth(), obstacle->GetCollisionHeight(), obstacle->GetCollisionDepth()}
		);

		if (Collision::CheckAABB(playerAABB, obstacleAABB)) {
			if (gameState_ == GameState::Playing) {
				// 衝突！ゲームオーバー
				gameState_ = GameState::GameOver;
				stageSettings_->SetGameOver(true);
				PostEffect::SetActivePostEffect(PostEffect::Type::GrayScale);
			}
			
			// 衝突状態をセット（赤くする。ポーズ中やゲームオーバー時も表示を維持）
			playerData.SetCollision(obstacleData.id);
			obstacleData.SetCollision(playerData.id);
		}
		
		HapiColi::HapiColi::GetInstance().RecordObject(obstacleData);
	}
	HapiColi::HapiColi::GetInstance().RecordObject(playerData);
}