#pragma once
#include <Engine.h>

class Enemy
{

public:
	~Enemy();

	void ImGui();

	void Initialize();

	void Update(Matrix4x4 viewMatrix);

	void Draw();

};

