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
	bool isSelect_ = false;

	MenuCommand menuCommand_ = kReturnToGame;

	std::unique_ptr<Sprite> dimming_ = std::make_unique<Sprite>();
	Vector4 dimmingColor_ = { 0.0f,0.0f,0.0f,0.7f };


public:

	void ImGui();

	void Initialize( );

	void Update( );

	void Draw();

	bool IsPause() { return isPause_; }
	bool IsSelect() { return isSelect_; }
};

