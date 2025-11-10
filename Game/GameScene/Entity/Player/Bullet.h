#pragma once
#include <memory>
#include <Engine.h>

class Player;
class Enemy;

class Bullet
{
private:

	std::unique_ptr<Model> model_;
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Vector3 velocity_;
	bool speed_ = 1.5f;
	bool isActiv_;

	float fream_=1.0f;

	Vector2 size_ = { 0.8f,0.8f };

public:

	void ImGui();

	void Initialize();

	void Update(Matrix4x4 viewMatrix);

	void Draw();

	void IsShot(Player *player);

	void OnCollision(const Enemy* enemy);

	AABB GetAABB();
	bool IsActiv() { return isActiv_; }


};

