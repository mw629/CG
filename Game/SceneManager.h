#pragma once
#include "SceneGame.h"
#include <memory>

class SceneManager
{
private:

	std::unique_ptr<SceneGame> gameScene_;

public:

	void ImGui();

	void Initialize();

	void Update();

	void Draw();

};

