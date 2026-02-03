#pragma once
#include <memory>
#include <Engine.h>
#include "../../Entity.h"

class MapChipField;
class Player;
class Enemy;
class BOSS;

class Bullet :public Entity
{
protected:

	
	float speed_ = 1.5f;
	float fream_ = 1.0f;


public:

	void ImGui();

	virtual void Initialize(MapChipField* mapChipField) {};

	virtual void Update(Matrix4x4 viewMatrix) {};

	virtual void Draw() {};

	virtual void IsShot(Player* player) {};

	virtual void OnCollision(const Enemy* enemy) {};
	virtual void OnCollision(const BOSS* boss) {};

	bool IsActiv() { return isActive_; }


};

