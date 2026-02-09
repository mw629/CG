#pragma once
#include <Engine.h>
#include "SceneManager.h"


class GameApplication
{
private:

	std::unique_ptr<Engine> engine;
	std::unique_ptr<SceneManager> sceneManager;

public:
	GameApplication(int32_t kClientWidth, int32_t kClientHeight);

	void Run();
};

