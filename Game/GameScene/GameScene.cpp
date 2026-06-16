#include "GameScene.h"
#include <imgui.h>
#include <memory>
#include <Engine.h>
#include "../../Editer/EditorManager.h"
#include "AssetManager.h"

GameScene::~GameScene()
{
}

void GameScene::ImGui()
{
#ifdef _USE_IMGUI
	ImGui::Begin("GameScene");

	camera_.get()->ImGui();

	if (ImGui::CollapsingHeader("Game State", ImGuiTreeNodeFlags_DefaultOpen)) {
		const char* stateNames[] = { "Playing", "Paused", "GameClear", "GameOver", "Editor" };
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
		if (ImGui::TreeNode("Top 3 Ranking")) {
			for (int i = 0; i < 3; i++) {
				if (topRankings_[i] > 0.0f) {
					ImGui::Text("Rank %d: %.2f m", i + 1, topRankings_[i]);
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
			player_->Reset();
			currentDistance_ = 0.0f;
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

		float interval = stageSettings_->GetObstacleInterval();
		if (ImGui::SliderFloat("Obstacle Interval", &interval, 5.0f, 50.0f)) {
			stageSettings_->SetObstacleInterval(interval);
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

		ImGui::Text("Your Score: %.2f m", currentDistance_);
		ImGui::Separator();
		ImGui::Text("--- TOP 3 RANKING ---");
		for (int i = 0; i < 3; i++) {
			if (topRankings_[i] > 0.0f) {
				ImGui::Text("  %d. %.2f m", i + 1, topRankings_[i]);
			} else {
				ImGui::Text("  %d. ---", i + 1);
			}
		}
		ImGui::Separator();

		if (ImGui::Button("Restart (1)", ImVec2(200, 40))) {
			gameState_ = GameState::Playing;
			stageSettings_->Reset();
			PostEffect::SetActivePostEffect(PostEffect::Type::Normal);
			player_->Reset();
			currentDistance_ = 0.0f;
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



	// camera_->SetDebugCamera() は上記で設定済み
	camera_->SetTransform(cameraTransform_);
	camera_->Update();

	// スカイボックスの初期化
	skyBoxTexture_ = texture_.get()->CreateTexture("resources/DDS/SnowWorld.dds");
	skyBox_.get()->Initialize(skyBoxTexture_);
	skyBox_.get()->SetShader("SkyBoxShader");
	skyBox_.get()->SetLighting(false);
	skyBox_.get()->SetTransform(skyBoxTransform_);
	skyBox_.get()->name_ = "SkyBox";

	// プレイヤーの初期化
	ModelData modelData = AssimpLoadObjFile("resources/Model/Player", "player.obj");
	player_->Initialize(modelData);

	// オブジェクトマネージャーへの登録
	gameObjectManager_->Clear();
	auto skyboxRenderObj = std::make_shared<RenderObject>(skyBox_);
	skyboxRenderObj->SetName("SkyBox");
	gameObjectManager_->AddObject(skyboxRenderObj);
	player_->SetName("Player");
	gameObjectManager_->AddObject(player_);

	// ステージの初期化
	ModelData roadModelData = AssetManager::LoadModel("resources/Plane", "Plane.gltf");
	ModelData obstacleModelData = AssetManager::LoadModel("resources/Block", "Block.obj");
	stageSettings_->Initialize(roadModelData, obstacleModelData, gameObjectManager_.get());

	currentDistance_ = 0.0f;
}

void GameScene::Update() {

	PostEffect::SetActivePostEffect(PostEffect::Type::GaussianFilter);

	// Engine側のPlay/Stop状態に同期してゲームステートを切り替え
	bool isEnginePlaying = EditorManager::IsPlaying();
	if (isEnginePlaying && gameState_ == GameState::Editor) {
		gameState_ = GameState::Playing;
		camera_->SetDebugCamera(false);
	} else if (!isEnginePlaying && gameState_ == GameState::Playing) {
		gameState_ = GameState::Editor;
		camera_->SetDebugCamera(true);
	}

	camera_->Update();
	view = camera_->GetViewMatrix();

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
	else if(gameState_ == GameState::GameOver){
		// 1でリスタート
		if (Input::PushKey(DIK_1)) {
			gameState_ = GameState::Playing;
			stageSettings_->Reset();
			PostEffect::SetActivePostEffect(PostEffect::Type::Normal);
			player_->Reset();
			currentDistance_ = 0.0f;
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
}

void GameScene::Draw() {
	//カメラの設定
	Draw::SetCamera(camera_.get());
	//背景の設定
	Draw::SetEnvironmentTexture(skyBoxTexture_);

	// オブジェクトの一括描画（SkyBox, Player など）
	gameObjectManager_->DrawAll();

	// ステージ描画（道路 + 障害物）
	stageSettings_->Draw();

	// ポーズ中の描画
	if (gameState_ == GameState::Paused) {
		pauseSystem_->Draw();
	}


}

void GameScene::PlayingUpdate()
{
	float speedMultiplier = 1.0f;
	if (stageSettings_->GetBaseScrollSpeed() > 0.0f) {
		speedMultiplier = stageSettings_->GetScrollSpeed() / stageSettings_->GetBaseScrollSpeed();
	}

	currentDistance_ += stageSettings_->GetScrollSpeed();

	CheckKeepRolling();
	
	// オブジェクトの一括更新
	gameObjectManager_->UpdateAll(view, speedMultiplier);
	
	stageSettings_->Update(view);

	// 当たり判定チェック
	CheckCollisions();
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
		if (!obstacle->GetIsActive()) continue;

		AABB obstacleAABB = Collision::MakeAABB(
			obstacle->GetTransform(),
			obstacle->GetCollisionWidth(),
			obstacle->GetCollisionHeight(),
			obstacle->GetCollisionDepth()
		);

		if (Collision::CheckAABB(playerAABB, obstacleAABB)) {
			// 衝突！ゲームオーバー
			gameState_ = GameState::GameOver;
			stageSettings_->SetGameOver(true);
			PostEffect::SetActivePostEffect(PostEffect::Type::GrayScale);
			
			// ランキング更新
			UpdateRanking();
			
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

void GameScene::CheckKeepRolling()
{
	bool keepRolling = false;
	if (player_->GetIsRolling()) {
		// Calculate a "standing up" AABB for the player
		const Transform& playerTransform = player_->GetTransform();
		AABB standingAABB = Collision::MakeAABB(playerTransform, 0.8f, 1.5f, 0.8f);

		for (int i = 0; i < stageSettings_->GetMaxObstacles(); i++) {
			Obstacle* obstacle = stageSettings_->GetObstacle(i);
			if (!obstacle->GetIsActive()) continue;

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