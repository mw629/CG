#pragma once
#include "GameScene.h"
#include <memory>
#include "IScene.h"
#include "Map/MapManager.h"

class SceneManager
{
private:

	IScene* scene_;
	std::unique_ptr<MapManager> mapManager_;

public:
	SceneManager();
	~SceneManager();

	void ImGui();

	void Initialize();

	void Update();

	void Draw();

	IScene* CreateScene(int sceneID);

};

