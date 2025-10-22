#pragma once
#include "../Entity.h"

class Enemy:public Entity
{

public:
	~Enemy()override;

	void ImGui()override;

	void Initialize()override;

	void Update(Matrix4x4 viewMatrix)override;

	void Draw()override;

};

