#pragma once
#include "GameScene.h"
#include <memory>

class SceneManager
{
private:

	std::unique_ptr<GameScene> gameScene_;

public:

	void Initialize();

	void Update();

	void Draw();

};

