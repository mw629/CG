#include "Playing.h"
#include "Map/MapManager.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif



Playing::~Playing()
{
	for (Bullet* bullet : bullet_) {
		if (bullet) {
			delete bullet;
		}
	}
	for (Enemy* enemy : enemy_) {
		if (enemy) {
			delete enemy;
		}
	}
}

void Playing::ImGui() {
#ifdef USE_IMGUI
	camera_.get()->ImGui();
	player_.get()->ImGui();

	int bulletNum = 0;
	for (Bullet* bullet : bullet_) {
		if (bullet->IsActiv()) {
			bulletNum++;
		}
	}
	ImGui::DragInt("BulletNum", &bulletNum);
#endif
}

void Playing::Initialize(int stage) {

	//生成
	camera_ = std::make_unique<TrackingCamera>();

	mapchip_ = std::make_unique<Block>();
	goal_ = std::make_unique<Goal>();

	player_ = std::make_unique<Player>();

	mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_.get()->SetMapData(MapManager::MapData(stage));//stage数を引数に入れる

	//初期化
	camera_.get()->Initialize(player_.get());
	viewMatrix_ = camera_.get()->GetView();

	mapchip_.get()->Initialize(mapChipField_.get(), viewMatrix_);

	player_.get()->Initialize(mapChipField_.get()->GetPlayerSpawn(), viewMatrix_);
	player_.get()->SetMapChipField(mapChipField_.get());

	goal_.get()->Initialize(mapChipField_.get()->GetGoalPosition(), viewMatrix_);

	for (int i = 0; i < bulletNum_; i++) {
		Bullet* newBullet = new NormalBullet();
		newBullet->Initialize(mapChipField_.get());
		bullet_.push_back(newBullet);
	}

	EnemySpawn();

	HP_ = std::make_unique<HP>();
	HP_.get()->Initialize();
	operation_ = std::make_unique<Operation>();
	operation_.get()->Initialize();

	// フラグの初期化
	isGoal_ = false;
	isGameOver_ = false;
}


void Playing::Update() {

	CheckAllCollisions();

	player_.get()->Update(viewMatrix_);
	HP_.get()->Update(player_.get()->GetHP());
	operation_.get()->Update();

	if (player_.get()->IsShot()) {
		for (Bullet* bullet : bullet_) {
			if (!bullet->IsActiv()) {
				bullet->IsShot(player_.get());
				break;
			}
		}
	}
	for (Bullet* bullet : bullet_) {
		bullet->Update(viewMatrix_);
	}
	for (Enemy* enemy : enemy_) {
		enemy->Update(viewMatrix_);
	}

	mapchip_.get()->Update(viewMatrix_);
	goal_.get()->Update(viewMatrix_);

	camera_.get()->Update();
	viewMatrix_ = camera_.get()->GetView();
}
void Playing::Draw() {

	HP_.get()->Draw();
	operation_.get()->Draw();

	for (Enemy* enemy : enemy_) {
		enemy->Draw();
	}

	player_.get()->Draw();

	for (Bullet* bullet : bullet_) {
		bullet->Draw();
	}
	

	mapchip_.get()->Draw();
	goal_.get()->Draw();
}

void Playing::EnemySpawn()
{
	for (const auto& enemyPos : mapChipField_.get()->GetEnemyPosition(0)) {
		Enemy* newEnemy = new StopEnemy();
		newEnemy->Initialize(enemyPos, viewMatrix_);
		newEnemy->SetMapChipField(mapChipField_.get());
		enemy_.push_back(newEnemy);
	}
	for (const auto& enemyPos : mapChipField_.get()->GetEnemyPosition(1)) {
		Enemy* newEnemy = new RunEnemy();
		newEnemy->Initialize(enemyPos, viewMatrix_);
		newEnemy->SetMapChipField(mapChipField_.get());
		enemy_.push_back(newEnemy);
	}
	for (const auto& enemyPos : mapChipField_.get()->GetEnemyPosition(2)) {
		Enemy* newEnemy = new JumpEnemy();
		newEnemy->Initialize(enemyPos, viewMatrix_);
		newEnemy->SetMapChipField(mapChipField_.get());
		enemy_.push_back(newEnemy);
	}

}


bool Playing::IsCollision(const AABB& aabb1, const AABB& aabb2)
{
	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x &&
		aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y &&
		aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) {
		return true;
	}
	return false;
}

void Playing::CheckAllCollisions() {
#pragma region 自キャラとゴール・コインの当たり判定
	AABB aabb1, aabb2;

	aabb1 = player_.get()->GetAABB();
	aabb2 = goal_.get()->GetAABB();
	if (IsCollision(aabb1, aabb2)) {
		isGoal_ = true;
	}

	// 死亡していない場合のみ敵との衝突判定を行う
	if (!player_.get()->IsDead()) {
		for (Enemy* enemy : enemy_) {
			// 死亡演出中または非アクティブな敵はスキップ
			if (!enemy->IsActive() || enemy->IsPlayingDeathAnimation()) {
				continue;
			}
			aabb2 = enemy->GetAABB();
			if (IsCollision(aabb1, aabb2)) {
				player_.get()->OnCollision(enemy);
			}
		}
	}

	if (player_.get()->IsDeathAnimationFinished()) {
		isGameOver_ = true;
	}

	for (Bullet* bullet : bullet_) {

		aabb1 = bullet->GetAABB();
		for (Enemy* enemy : enemy_) {
			// 死亡演出中または非アクティブな敵はスキップ
			if (!enemy->IsActive() || enemy->IsPlayingDeathAnimation()) {
				continue;
			}
			if (!bullet->IsActiv()) {
				continue;
			}
			aabb2 = enemy->GetAABB();
			if (IsCollision(aabb1, aabb2)) {
				bullet->OnCollision(enemy);
				enemy->OnCollision(bullet);
			}
		}
	}

#pragma endregion
}
