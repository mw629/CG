#pragma once

#include <memory>
#include <Engine.h>



class Player
{
private:

	
	enum MoveDirection
	{
		Left,
		Right,
		None
	};


	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	Transform transform_;

	//レーン移動のための変数
	int laneIndex_ = 0;// 現在のレーン位置
	int targetLaneIndex_ = 0;// 目標のレーン位置

	float laneChangeSpeed_ = 1.0f; // レーン移動の速度
	MoveDirection moveDirection_ = MoveDirection::None; // 移動方向


public:

	Player();
	~Player();

	void Initialize();

	void Update(Matrix4x4 view);

	void PlayerMove();

	void Draw();


};

