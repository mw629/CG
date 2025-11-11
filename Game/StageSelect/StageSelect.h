#pragma once
#include <Engine.h>
#include "../IScene.h"

class StageSelect :public IScene
{
private:

public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

