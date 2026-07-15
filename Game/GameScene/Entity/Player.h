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

	float laneChangeSpeed_ = 0.2f; // レーン移動の速度
	float lerpTime_ = 0.0f; // 補間用タイマー
	float startX_ = 0.0f;   // 移動開始時のX座標
	MoveDirection moveDirection_ = MoveDirection::None; // 移動方向

	// レーン設定
	int minLane_ = -1;
	int maxLane_ = 1;
	float laneWidth_ = 2.0f;

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

	// 各アクションの硬直（クールタイム）用変数
	float laneChangeRecovery_ = 0.0f; // レーン移動終了後の硬直フレーム数
	float jumpRecovery_ = 0.0f;       // ジャンプ着地後の硬直フレーム数
	float rollRecovery_ = 0.0f;       // 転がり終了後の硬直フレーム数
	float currentRecoveryTimer_ = 0.0f; // 現在の硬直タイマー

	// ヒット時の演出用変数
	bool isHit_ = false;
	bool isTrip_ = false;
	float hitTimer_ = 0.0f;
	float hitDuration_ = 90.0f; // ノックバックにかかるフレーム数
	Vector3 knockbackVelocity_{ 0.0f, 0.0f, 0.0f }; // ノックバック速度

	// 強制中央移動用の変数
	bool isForcedCentering_ = false;
	float forcedCenterTimer_ = 0.0f;
	float forcedCenterDuration_ = 30.0f; // 中央に到達するまでのフレーム数
	float forcedCenterStartX_ = 0.0f;

public:

	Player();
	~Player();

	void Initialize(ModelData modelData);
	void Reset();

	void Update(Matrix4x4 view, float speedMultiplier = 1.0f) override;

	void PlayerMove(float speedMultiplier);
	void HitUpdate(float speedMultiplier);

	void SetLaneLimits(int minLane, int maxLane, float laneWidth) {
		minLane_ = minLane;
		maxLane_ = maxLane;
		laneWidth_ = laneWidth;
	}

	void Draw(class Draw& draw) override;
	void ImGuiInnerComponents() override;

	bool HasMaterial() const override {
		return model_ && model_->GetComponent<MaterialComponent>() != nullptr || GameObject::HasMaterial();
	}

	// GetTransform() is inherited from GameObject
	bool GetIsRolling() const { return isRolling_; }
	void SetKeepRolling(bool keep) { keepRolling_ = keep; }

	// ヒット演出用
	void OnHit(bool isTrip = false);
	bool IsHitAnimationFinished() const;

	// 狭まる区間で強制的に中央へ寄せる
	void StartForceToCenter(float duration);
};

