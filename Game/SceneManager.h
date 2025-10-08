#pragma once
#include "SceneGame.h"
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

};

