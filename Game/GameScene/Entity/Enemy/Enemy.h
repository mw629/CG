#pragma once
#include <vector>
#include <Engine.h>
#include "../../Map/MapChipField.h"
#include "../Entity.h"

class Player;
class Bullet;

class Enemy: public Entity{

	
protected:

	ModelData modelData_;

	bool isDead_ = false;
	bool isPlayingDeathAnimation_ = false;
	float deathAnimationTimer_ = 0.0f;
	const float kDeathAnimationDuration_ = 1.0f;

	float moveSpeed_ = 0.05f; 
	float moveRange_ = 0.3f;  
	float moveTimer_ = 0.0f;

	Vector3 initialScale_ = {1.0f, 1.0f, 1.0f};

public:

	virtual void Initialize(const Vector3& position,Matrix4x4 viewMatrix);

	virtual void Update(Matrix4x4 viewMatrix);

	void DeathAnimation();

	void Draw();

	bool IsActive() { return isActive_; }
	bool IsDead() const { return isDead_; }
	bool IsPlayingDeathAnimation() const { return isPlayingDeathAnimation_; }

	void OnCollision(const Bullet* bullet);

};