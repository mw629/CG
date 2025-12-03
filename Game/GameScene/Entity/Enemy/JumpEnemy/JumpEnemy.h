#pragma once
#include "../Enemy.h"
#include <Engine.h>

class JumpEnemy:public Enemy
{
private:


	bool canJump;
	float JumpCoolTimeMax = 2.0f;
	float JumpCoolTime = 2.0f;
	float kJumpAcceleration = 0.25f;

	float gravity_=0.7f;
	float kLimitFallSpeed = 0.1f;

	float runSpeed_=-1.0f;

public:

	void Initialize(const Vector3& position, Matrix4x4 viewMatrix)override;

	void Update(Matrix4x4 viewMatrix)override;

	void Move();

	void HitWall()override;

};

