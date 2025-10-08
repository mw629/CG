#pragma once
#include "IScene.h"

class TitleScene:public IScene
{
private:


public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;


};

