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

	if (ImGui::CollapsingHeader("Game Info")) {
		ImGui::Text("Game Over: %s", isGameOver_ ? "YES" : "NO");
		ImGui::Text("Player Y: %.2f", player_->GetTransform().translate.y);
		ImGui::Text("Rolling: %s", player_->GetIsRolling() ? "YES" : "NO");

		if (ImGui::Button("Reset Game")) {
			isGameOver_ = false;
			stageSettings_->Reset();
		}
	}

#endif // _USE_IMGUI
}

void GameScene::Initialize() {

	sceneID_ = SceneID::Game;

	camera_->SetDebugCamera(false); // ゲーム用カメラを使う
	camera_->SetTransform(cameraTransform_);
	camera_->Update();

	// プレイヤーの初期化
	ModelData modelData = AssimpLoadObjFile("resources/player", "player.obj");
	player_->Initialize(modelData);

	// ステージの初期化
	ModelData roadModelData = AssimpLoadObjFile("resources/Plane", "Plane.gltf");
	ModelData obstacleModelData = AssimpLoadObjFile("resources/Block", "Block.obj");
	stageSettings_->Initialize(roadModelData, obstacleModelData);
}

void GameScene::Update() {

	camera_->Update();
	Matrix4x4 view = camera_->GetViewMatrix();

	if (!isGameOver_) {
		player_->Update(view);
		stageSettings_->Update(view);

		// 当たり判定チェック
		CheckCollisions();
	}
	else {
		// ゲームオーバー中にキーでリスタート
		if (Input::PushKey(DIK_R)) {
			isGameOver_ = false;
			stageSettings_->Reset();
		}
	}
}

void GameScene::Draw() {
	//カメラの設定
	Draw::SetCamera(camera_.get());
	//背景の設定
	Draw::SetEnvironmentTexture(texture_.get()->CreateTexture("resources/rostock_laage_airport_4k.dds"));

	// ステージ描画（道路 + 障害物）
	stageSettings_->Draw();

	// プレイヤー描画
	player_->Draw();
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
			isGameOver_ = true;
			stageSettings_->SetGameOver(true);
			Engine::ChangePostEffect(Engine::PostEffectType::GrayScale);
			break;
		}
	}
}