#pragma once
#include <Engine.h>
#include "System/GameState.h"

class Pause
{
private:

	MenuCommand menuCommand_ = kReturnToGame;

public:

	void ImGui();

	void Initialize(Matrix4x4 viewMatrix);

	void Update(Matrix4x4 viewMatrix);

	void Draw();

};

