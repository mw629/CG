#pragma once
#include "Entity.h"

class Player :public Entity
{
private:

	//射撃用変数




public:
	~Player()override;

	//基本の関数
	void ImGui()override;

	void Initialize()override;

	void Update(Matrix4x4 viewMatrix)override;

	void Draw()override;

	//プレイヤーの動き
	void MoveInput();

};

