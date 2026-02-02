#pragma once
#include "../Entity.h"

class Player;
class Bullet;

class BOSS :public Entity
{
private:

	ModelData modelData_;

	int HP_ = 50;


	bool isActive_;

	int deathSE_ = -1;

public:

	void Initialize(const Vector3& position, Matrix4x4 viewMatrix);

	void Update(Player* player, Matrix4x4 viewMatrix);

	void Draw();

	void OnCollision(const Bullet* bullet);
};

