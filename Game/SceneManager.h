#pragma once
#include "GameScene.h"
#include <memory>
#include "IScene.h"

class SceneManager
{
private:

	std::unique_ptr<IScene> scene_;

public:
	SceneManager();
	~SceneManager() = default;

	void ImGui();

	void Initialize();

	void Update();

	void Draw();

	void Run();

	std::unique_ptr<IScene> CreateScene(int sceneID);

};

