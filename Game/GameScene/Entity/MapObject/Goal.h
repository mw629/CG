#pragma once
#include <memory>
#include <Engine.h>

class Player;

class Goal
{
private:


	ModelData modelData_;
	std::unique_ptr<Model> model_;
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	// キャラクターの当たり判定サイズ
	Vector2 size_ = { 1.0f,1.0f };
	bool isActive_ = true;


public:

	void Initialize(const Vector3& position, Matrix4x4 viewMatrix);

	void Update(Matrix4x4 viewMatrix);

	void Draw();

	bool IsActive() { return isActive_; }

	AABB GetAABB();

};

