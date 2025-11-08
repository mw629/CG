#pragma once
#include <Engine.h>
#include "System/GameState.h"

class Pause
{
private:



	MenuCommand menuCommand_ = kReturnToGame;

public:

	void ImGui();

	void Initialize( );

	void Update( );

	void Draw();

};

