#pragma once
#include "GameScene.h"
#include <memory>
#include "IScene.h"

class SceneManager
{
private:

	IScene* scene_;

public:
	SceneManager();
	~SceneManager();

	void ImGui();

	void Initialize();

	void Update();

	void Draw();

	IScene* CreateScene(int sceneID);

};

