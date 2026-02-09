#pragma once
#include "../Enemy.h"

class Player;
class Bullet;

class BOSS : public Enemy
{
private:
	int HP_ = 1;
	Vector3 ReferencePoint_;

	// ボスの状態
	enum class BossState {
		kIdle,
		kMoveToPlayerX,
		kFall,
		kReturnToReference
	};

	BossState bossState_ = BossState::kIdle;

	// 移動・イージング用
	float moveTimer_ = 0.0f;
	float moveDuration_ = 0.6f; // プレイヤーの x へ移動する時間（秒）
	float moveStartX_ = 0.0f;
	float moveTargetX_ = 0.0f;

	float fallFream_ = 0.0f;

	// 落下用（Entity::velocity_ を使用）
	float gravity_ = 1.2f;        // 重力加速度（任意調整）
	float maxFallSpeed_ = -2.0f;  // 端速（下向き）

	// 帰還用（二段階：縦移動 → 横移動）
	float returnTimer_ = 0.0f;
	// 縦に戻る時間（着地位置の y から ReferencePoint_.y へ）
	float returnVerticalDuration_ = 0.45f;
	// 横に戻る時間（ReferencePoint_.y に到達した状態で x を戻す）
	float returnHorizontalDuration_ = 0.45f;

	Vector3 returnStartPos_{};
	Vector3 returnTargetPos_{};

	enum class ReturnPhase {
		kAscendToReferenceY,
		kMoveHorizontallyToReferenceX
	};
	ReturnPhase returnPhase_ = ReturnPhase::kAscendToReferenceY;

	// 実行中フラグ（状態自動開始制御）
	bool actionStarted_ = false;

	float kDeltaTime = 1.0f / 60.0f; // フレーム時間（固定）

	// --- 関数化した処理 ---
	// イージング関数（InOut Cubic）
	float EaseInOutCubic(float t) const;

	// 状態ごとの開始処理と更新処理
	void StartMoveToPlayer(float playerX);
	void UpdateMoveToPlayer();

	void StartFall();
	void UpdateFall();

	void StartReturnToReference();
	void UpdateReturnToReference();

public:
	void Initialize(const Vector3& position, Matrix4x4 viewMatrix)override;

	// BOSS 固有の Update（Player ポインタを受け取る）
	void Update(Player* player, Matrix4x4 viewMatrix);

	// 外部から強制的に動作をリセットして再起動可能にする
	void ResetAction();

	// 外部から即時開始する（プレイヤー座標を指定して開始）
	void StartActionAtPlayerX(float playerX);

	void Attack();

	// Bullet 衝突処理
	void OnCollision(const Bullet* bullet)override;
};

