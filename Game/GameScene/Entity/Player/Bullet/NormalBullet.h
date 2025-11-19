#pragma once
#include "Bullet.h"

class NormalBullet :public Bullet
{

public:

	void Initialize(MapChipField* mapChipField)override;

	void Update(Matrix4x4 viewMatrix)override;

	void Draw()override;

	void IsShot(Player* player)override;

	void HitWall()override;

	void OnCollision(const Enemy* enemy)override;

};

