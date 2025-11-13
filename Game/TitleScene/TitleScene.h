#pragma once
#include <Engine.h>
#include "../IScene.h"
#include "../SkyDome.h"

class TitleScene:public IScene
{
private:

	std::unique_ptr<Sprite> backImage_;
	std::unique_ptr<Sprite> SpaceSprite_ = std::make_unique<Sprite>();

	std::unique_ptr<SkyDome> skyDome_;

public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;


};

