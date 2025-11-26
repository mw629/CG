#pragma once
#include <vector>
#include <Engine.h>
#include "../../Map/MapChipField.h"

class Player;
class Bullet;

class Enemy{

	
protected:

	ModelData modelData_;
	std::unique_ptr<Model> model_;
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	Vector4 color = { 1.0f,0.0f,0.0f,1.0f };

	// キャラクターの当たり判定サイズ
	Vector2 size_ = { 1.0f,1.0f };

	bool isActive_ = true;
	bool isDead_ = false;

	float velocity_;


	float moveSpeed_ = 0.05f; 
	float moveRange_ = 0.3f;  
	float moveTimer_ = 0.0f;

public:

	virtual void Initialize(const Vector3& position,Matrix4x4 viewMatrix);

	virtual void Update(Matrix4x4 viewMatrix);

	void Draw();

	bool IsActive() { return isActive_; }

	void OnCollision(const Bullet* bullet);

	AABB GetAABB();

};