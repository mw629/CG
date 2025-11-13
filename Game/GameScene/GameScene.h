#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>
#include "../IScene.h"
#include "Playing.h"
#include "Pause.h"
#include "System/GameState.h"
#include "../SkyDome.h"




class GameScene :public IScene
{
private:

	State gameState_=State::kPlaying;

	std::unique_ptr<Playing> playing_;
	std::unique_ptr<Pause> pause_;

	std::unique_ptr<SkyDome> skyDome_;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

	//状態ごとの更新
	void StateUpdate();

	//状態ごとの描画
	void StateDraw();

	void ChangeState();

	static void SetStage(int stage);

};

