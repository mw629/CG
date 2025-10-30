#pragma once
#include "System/GameState.h"
#include "Entity/Player/Player.h"
#include "Entity/Enemy/Enemy.h"
#include "Entity/MapObject/Block.h"
#include "Map/MapChipField.h"


class Playing
{
private:

	std::unique_ptr<MapChipField> mapChipField_;
	std::unique_ptr<Block> mapchip_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<Enemy> enemy_;

public:

	void ImGui();

	void Initialize(Matrix4x4 viewMatrix);

	void Update(Matrix4x4 viewMatrix);

	void Draw();
	

};

