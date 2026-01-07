#pragma once
#include <memory>
#include <Engine.h>
#include "../Entity.h"

class MapChipField;
class Goal;
class Enemy;


class Player : public Entity
{
private:

#pragma region enum

	enum class Behavior {
		kUnknown, // 変更リクエストなし
		kRoot,   // 通常状態
		kAttack, // 攻撃中
		kDeath,  // 死亡演出中
	};

	enum class AttackPhase {
		reservoir,
		rush,
		lingeringSound
	};

	// クリア演出フェーズ
	enum class ClearPhase {
		kWaitLanding,  // 着地待ち
		kTurnToCamera, // 画面向き変更
		kBackflip,     // バク転
		kFinished      // 完了
	};

#pragma endregion



	int HP_ = 3;
	float speed_ = 0.1f;

	bool isShot_;
	bool isDead_ = false;

	float invincibleTime = 90.0f;
	int invincibleFream = 0;

	// ノックバック用パラメータ
	bool isKnockback_ = false;
	float knockbackTimer_ = 0.0f;
	Vector3 knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
	static inline const float kKnockbackDuration = 0.3f;
	static inline const float kKnockbackForceX = 0.1f;
	static inline const float kKnockbackForceY = 0.1f;



	// 固定の加速度
	static inline const float kAcceleration = 0.9f;
	// 固定の減衰抵抗力
	static inline const float kAttenuation = 0.08f;
	// 最大速度制限
	static inline const float kLimitRunSpeed = 0.1f;



	// 重力加速度(下方向)
	float kGravityAccleration = 0.7f;
	// 最大落下速度(下方向)
	float kLimitFallSpeed = 0.5f;
	//壁との衝突時の摩擦係数
	float wallFrictio = 0.3f;
	// ジャンプ初速(上方向)
	float kJumpAcceleration = 0.25f;
	
	//ダブルジャンプのフラグ
	bool doubleJump = false;

	// コヨーテタイム用
	float coyoteTime_ = 0.0f;
	static inline const float kCoyoteTimeDuration = 0.1f;

#pragma region Animetion
	
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	//ジャンプ用
	bool isJump_;
	float animationFream_ = 0.0f;

	// 死亡演出用
	float deathAnimationTimer_ = 0.0f;
	static inline const float kDeathAnimationDuration = 1.0f;

	// クリア演出用
	bool isClear_ = false;
	ClearPhase clearPhase_ = ClearPhase::kWaitLanding;
	float clearAnimationTimer_ = 0.0f;
	static inline const float kTurnToCameraDuration = 0.3f;
	static inline const float kBackflipDuration = 0.8f;
	float clearTurnStartRotationY_ = 0.0f;

#pragma endregion

#pragma region Audio

	// ジャンプ効果音
	int jumpSE_ = -1;
	// ダメージ効果音
	int damageSE_ = -1;

#pragma endregion

public:
	void ImGui();

	void Initialize(const Vector3& position, Matrix4x4 viewMatrix);

	void Update(Matrix4x4 viewMatrix);

	void Draw();

	//入力処理
	void MoveInput();

	void JumpAnimation();
	void DeathAnimation();
	void ClearAnimation();

	void HitWall()override;
	void OnGround()override;

	void OnCollision(const Goal* goal);
	void OnCollision(const Enemy* enemy);

	Vector3& GetVelocity() { return velocity_; };
	int GetHP() { return HP_; };
	bool IsShot() { return isShot_; }

	// デスフラグのgetter
	bool IsDead() const { return isDead_; };
	void SetDead(bool flag) { isDead_ = flag; };

	// 死亡演出が完了したかどうか
	bool IsDeathAnimationFinished() const { return isDead_ && deathAnimationTimer_ >= kDeathAnimationDuration; }

	// クリア関連
	bool IsClear() const { return isClear_; }
	void SetClear(bool flag) { isClear_ = flag; clearPhase_ = ClearPhase::kWaitLanding; clearAnimationTimer_ = 0.0f; }
	bool IsClearAnimationFinished() const { return isClear_ && clearPhase_ == ClearPhase::kFinished; }

};

