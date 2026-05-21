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

	float laneChangeSpeed_ = 0.1f; // レーン移動の速度
	float lerpTime_ = 0.0f; // 補間用タイマー
	float startX_ = 0.0f;   // 移動開始時のX座標
	MoveDirection moveDirection_ = MoveDirection::None; // 移動方向

	// アクション用の変数
	bool isJumping_ = false;
	float velocityY_ = 0.0f;
	float gravity_ = 0.02f;
	float jumpPower_ = 0.4f;
	float baseHeight_ = 0.0f; // 地面の高さ（Y座標）

	bool isRolling_ = false;
	float rollTimer_ = 0.0f;
	float rollDuration_ = 30.0f; // 転がりの継続フレーム数

public:

	Player();
	~Player();

	void Initialize(ModelData modelData);
	void Reset();

	void Update(Matrix4x4 view);

	void PlayerMove();

	void Draw();

	const Transform& GetTransform() const { return transform_; }
	bool GetIsRolling() const { return isRolling_; }
};

