#pragma once
#include "../Entity.h"

class Player;
class Bullet;

class BOSS :public Entity
{
private:

	ModelData modelData_;

	int HP_ = 50;

public:

	void Initialize(Matrix4x4 viewMatrix);

	void Update(Player* player, Matrix4x4 viewMatrix);


	void OnCollision(const Bullet* bullet);
};

