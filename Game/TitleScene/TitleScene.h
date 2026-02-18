#pragma once
#include <Engine.h>
#include "IScene.h"

class TitleScene:public IScene
{
private:

	std::unique_ptr<Sprite> backImage_;
	Vector2 spritePos[2];

public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;


};

