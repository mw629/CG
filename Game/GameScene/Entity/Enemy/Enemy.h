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

	float moveSpeed_ = 0.05f; 
	float moveRange_ = 0.3f;  
	float moveTimer_ = 0.0f;

public:

	virtual void Initialize(const Vector3& position,Matrix4x4 viewMatrix);

	virtual void Update(Matrix4x4 viewMatrix);

	void Draw();

	bool IsActive() { return isActive_; }

	void OnCollision(const Bullet* bullet);

};