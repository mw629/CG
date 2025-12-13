#pragma once
#include "../Enemy.h"
#include <Engine.h>

class RunEnemy :public Enemy
{
private:


	float gravity_ = 0.7f;
	float kLimitFallSpeed = 0.1f;

	float runSpeed_ = -5.0f;

public:

	void Initialize(const Vector3& position, Matrix4x4 viewMatrix)override;

	void Update(Matrix4x4 viewMatrix)override;

	void Move();

	void HitWall()override;

};

