#pragma once
#include "GameScene.h"
#include <memory>

class SceneManager
{
private:

	std::unique_ptr<GameScene> gameScene_;

public:

	void ImGui();

	void Initialize();

	void Update();

	void Draw();

};

