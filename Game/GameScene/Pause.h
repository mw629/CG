#pragma once
#include <Engine.h>
#include "System/GameState.h"

class Pause
{
private:

	enum SelectMode
	{
		backGame,
		backSelectScene,
		gameEnd,
	};

	SelectMode selectMode_;
	bool isPause_=true;

	MenuCommand menuCommand_ = kReturnToGame;

public:

	void ImGui();

	void Initialize( );

	void Update( );

	void Draw();

	bool IsPause() { return isPause_; }
};

