#pragma once
#include <Engine.h>
#include "../IScene.h"
#include "../SkyDome.h"

class StageSelect :public IScene
{
private:

	std::unique_ptr<Sprite> sprite_[3];
	std::unique_ptr<Sprite> SpaceSprite_ = std::make_unique<Sprite>();

	std::unique_ptr<SkyDome> skyDome_;

	int stageNum_=0;

public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

