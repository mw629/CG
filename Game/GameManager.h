#pragma once
#include <memory>
#include "GameScene/Map/MapManager.h"
#include "Direction/Fade.h"
#include "IScene.h"

class GameManager
{
private:

	IScene* scene_;
	std::unique_ptr<MapManager> mapManager_;

	std::unique_ptr<Fade> fade_;


public:
	GameManager();
	~GameManager();

	void ImGui();

	void Initialize();

	void Update();

	void Draw();

	IScene* CreateScene(int sceneID);

};

