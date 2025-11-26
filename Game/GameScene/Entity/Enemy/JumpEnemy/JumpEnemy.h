#pragma once
#include "../Enemy.h"
#include <Engine.h>

class JumpEnemy:public Enemy
{
private:

public:

	void Initialize(const Vector3& position, Matrix4x4 viewMatrix) override;


};

