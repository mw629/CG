#pragma once
#include <Engine.h>
#include <GameObjects/Camera.h>
#include "../IScene.h"
#include "Entity/Player/Player.h"
#include "Entity/Enemy/Enemy.h"
#include "Entity/MapObject/Block.h"
#include "Map/MapChipField.h"




class GameScene :public IScene
{
private:


	enum State {
		kGame,
		kPose,
	};

	enum MenuCommand {
		kReturnToGame,
		kRestart,
		kReturnToStageSelect,
	};

	State GameState_ = kGame;
	MenuCommand menuCommand_ = kReturnToGame;


	std::unique_ptr<Camera>camera_;
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{12.0f,0.0f,-50.0f} };

	std::unique_ptr<MapChipField> mapChipField_;
	std::unique_ptr<Block> mapchip_;

	std::unique_ptr<Player> player_;

	std::unique_ptr<Enemy> enemy_;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;


	//ゲーム用の関数

	void GameState();


	//ポーズ用の関数

	void PoseState();

	void PoseInput();

};

