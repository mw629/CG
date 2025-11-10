#include "Playing.h"
#include "Map/MapManager.h"
#include <imgui.h>




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

	camera_.get()->ImGui();
	player_.get()->ImGui();

	int bulletNum=0;
	for (Bullet* bullet : bullet_) {
		if (bullet->IsActiv()) {
			bulletNum++;
		}
	}
	ImGui::DragInt("BulletNum", &bulletNum);

}

void Playing::Initialize() {

	//生成
	camera_ = std::make_unique<TrackingCamera>();

	mapchip_ = std::make_unique<Block>();
	goal_ = std::make_unique<Goal>();

	player_ = std::make_unique<Player>();

	mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_.get()->SetMapData(MapManager::MapData(0));//stage数を引数に入れる

	camera_.get()->Initialize(player_.get());
	viewMatrix_ = camera_.get()->GetView();


	mapchip_.get()->Initialize(mapChipField_.get(), viewMatrix_);

	player_.get()->Initialize(mapChipField_.get()->GetPlayerSpawn(), viewMatrix_);
	player_.get()->SetMapChipField(mapChipField_.get());

	goal_.get()->Initialize(mapChipField_.get()->GetGoalPosition(), viewMatrix_);

	for (int i = 0; i < bulletNum_; i++) {
		Bullet *newBullet = new Bullet();
		newBullet->Initialize();
		bullet_.push_back(newBullet);
	}

	EnemySpawn();
}


void Playing::Update() {

	CheckAllCollisions();

	player_.get()->Update(viewMatrix_);

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

	player_.get()->Draw();

	for (Bullet* bullet : bullet_) {
		bullet->Draw();
	}
	for (Enemy* enemy : enemy_) {
		enemy->Draw();
	}

	mapchip_.get()->Draw();
	goal_.get()->Draw();
}

void Playing::EnemySpawn()
{
	for (const auto& enemyPos : mapChipField_.get()->GetEnemyPosition()) {
		Enemy* newEnemy = new Enemy();
		newEnemy->Initialize(enemyPos,viewMatrix_);
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
	for (Enemy* enemy : enemy_) {
		if (!enemy->IsActive()) {
			continue;
		}
		aabb2 = enemy->GetAABB();
		if (IsCollision(aabb1, aabb2)) {
			player_.get()->OnCollision(enemy);
			if (player_.get()->IsDead()) {
				isGameOver_ = true;
			}
		}
	}

	for (Bullet* bullet : bullet_) {
		aabb1 = bullet->GetAABB();
		if (!bullet->IsActiv()) {
			continue;
		}
		for (Enemy* enemy : enemy_) {
			if (!enemy->IsActive()) {
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
