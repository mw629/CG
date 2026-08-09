#pragma once
#include <Engine.h>
#include "../IScene.h"
#include "Camera.h"

class ResultScene : public IScene
{
private:
	std::unique_ptr<Sprite> backImage_;
	SpriteData spriteData_;
	Vector2 spritePos[2];
	std::unique_ptr<Camera> camera_;

public:
	void ImGui() override;
	void Initialize() override;
	void Update() override;
	void Draw(class Draw& draw) override;
};
