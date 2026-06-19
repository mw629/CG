#pragma once

#include <memory>
#include <Engine.h>

#include "GameObject.h"

class Player : public GameObject
{
private:

	
	enum MoveDirection
	{
		Left,
		Right,
		None
	};


	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	// transform_ is inherited from GameObject

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
	float gravity_ = 0.007f;
	float jumpPower_ = 0.17f;
	float baseHeight_ = 3.0f; // 地面の高さ（Y座標）

	bool isRolling_ = false;
	float rollTimer_ = 0.0f;
	float rollDuration_ = 45.0f; // 転がりの継続フレーム数
	bool keepRolling_ = false; // 強制的にしゃがみを維持するフラグ

	// ヒット時の演出用変数
	bool isHit_ = false;
	float hitTimer_ = 0.0f;
	float hitDuration_ = 90.0f; // ノックバックにかかるフレーム数
	Vector3 knockbackVelocity_{ 0.0f, 0.0f, 0.0f }; // ノックバック速度

public:

	Player();
	~Player();

	void Initialize(ModelData modelData);
	void Reset();

	void Update(Matrix4x4 view, float speedMultiplier = 1.0f) override;

	void PlayerMove(float speedMultiplier);
	void HitUpdate(float speedMultiplier);

	void Draw() override;
	void ImGui() override;

	// GetTransform() is inherited from GameObject
	bool GetIsRolling() const { return isRolling_; }
	void SetKeepRolling(bool keep) { keepRolling_ = keep; }

	// ヒット演出用
	void OnHit();
	bool IsHitAnimationFinished() const;
};

