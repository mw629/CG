#pragma once

#include "System/GameState.h"
#include "System/TrackingCamera.h"

#include "Map/MapChipField.h"

#include "Entity/Player/Player.h"
#include "Entity/Player/Bullet/NormalBullet.h"

#include "Entity/Enemy/Enemy.h"
#include "Entity/Enemy/JumpEnemy/JumpEnemy.h"
#include "Entity/Enemy/StopEnemy/StopEnemy.h"
#include "Entity/Enemy/RunEnemy/RunEnemy.h"
#include "Entity/Enemy/BOSS/BOSS.h"

#include "Entity/MapObject/Block.h"
#include "Entity/MapObject/Goal.h"
#include "UI/HP.h"
#include "UI/Operation.h"



class Playing
{
private:

	std::unique_ptr<TrackingCamera> camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{12.0f,0.0f,-50.0f} };
	Matrix4x4 viewMatrix_;

	std::unique_ptr<MapChipField> mapChipField_;
	std::unique_ptr<Block> mapchip_;

	std::unique_ptr<Player> player_;
	std::list<Bullet*> bullet_;
	int bulletNum_ = 10;

	std::unique_ptr<Goal> goal_;
	
	std::list<Enemy*> enemy_;

	bool isGoal_;
	bool isGameOver_;


	std::unique_ptr<HP> HP_;
	std::unique_ptr<Operation> operation_;


public:
	~Playing();

	void ImGui();

	void Initialize(int stage);

	void Update();

	void Draw();

	void EnemySpawn();

	bool IsCollision(const AABB& aabb1, const AABB& aabb2);

	void CheckAllCollisions();
	
	bool IsGoal() { return isGoal_; }
	bool IsGameOver() { return isGameOver_; }
};

