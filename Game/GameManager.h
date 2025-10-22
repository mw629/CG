#pragma once
#include <memory>
#include "GameScene/Map/MapManager.h"
#include "IScene.h"

class GameManager
{
private:

	IScene* scene_;
	std::unique_ptr<MapManager> mapManager_;

public:
	GameManager();
	~GameManager();

	void ImGui();

	void Initialize();

	void Update();

	void Draw();

	IScene* CreateScene(int sceneID);

};

