#pragma once
#include <memory>
#include <Engine.h>

class MapChipField;
class Goal;
class Coin;


class Player
{
private:

	std::unique_ptr<Model> model_;
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };


	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	float speed_ = 0.1f;


	//// 構造体 ////

	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceilingCollisionFlag = false; // 天井衝突フラグ
		bool landingFlag = false;          // 着地フラグ
		bool wallContactFlag = false;      // 壁接触フラグ
		Vector3 velocity;                  // 移動量
	};

	//// 列挙型 ////
	enum class LRDirection {
		kRight,
		kLeft,
	};

	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上
		kNumCorner    // 要素数
	};

	enum class Behavior {
		kUnknown, // 変更リクエストなし
		kRoot,   // 通常状態
		kAttack, // 攻撃中
	};

	enum class AttackPhase {
		reservoir,
		rush,
		lingeringSound
	};

	//// 変数 ////
	// 固定の加速度
	static inline const float kAcceleration = 0.9f;
	// 固定の減衰抵抗力
	static inline const float kAttenuation = 0.08f;
	// 最大速度制限
	static inline const float kLimitRunSpeed = 0.1f;
	// 向き
	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;
	// 設置状態フラグ
	bool onGround_ = true;
	// 重力加速度(下方向)
	float kGravityAccleration = 0.9f;
	// 最大落下速度(下方向)
	float kLimitFallSpeed = 0.5f;
	//壁との衝突時の摩擦係数
	float wallFrictio = 0.3f;
	// ジャンプ初速(上方向)
	float kJumpAcceleration = 0.305f;
	// 接地フラグ
	bool landing = false;
	//ダブルジャンプのフラグ
	bool doubleJump=false;
	//壁キックの初速度
	float kWallJumpAcceleration = 0.3f;
	//壁ジャンプ可能フラグ
	bool wallKick = false;
	//壁ジャンプ中のフラグ
	bool isWallJumping = false;
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;
	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;
	// デスフラグ
	bool isDead_ = false;
public:
	void ImGui();

	void Initialize(const Vector3& position, Matrix4x4 viewMatrix);

	void Update(Matrix4x4 viewMatrix);

	void Draw();

	//入力処理
	void MoveInput();

	// マップ衝突判定
	void MapCollisionDetection(CollisionMapInfo& info);
	void MapCollisionDetectionUp(CollisionMapInfo& info);
	void MapCollisionDetectionDown(CollisionMapInfo& info);
	void MapCollisionDetectionLeft(CollisionMapInfo& info);
	void MapCollisionDetectionRight(CollisionMapInfo& info);
	// 指定した角の座標を得る
	Vector3 CornerPosition(const Vector3& center, Corner corner);
	// 天井に接触しているときの処理
	void touchingTheCeiling(const CollisionMapInfo& info);
	// 設置状態の切り替え処理
	void SwitchingInstallationStatus(const CollisionMapInfo& info);
	// 壁に接触している場合の処理
	void contactWithAWall(const CollisionMapInfo& info);


	Transform& GetWorldTransform() { return transform_; };
	Vector3& GetVelocity() { return velocity_; };
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };
	//AABB GetAABB();
	// デスフラグのgetter
	bool IsDead() const { return isDead_; };
	void SetDead(bool flag) { isDead_ = flag; };
	
	void OnCollision(const Goal* goal);
	void OnCollision(const Coin* coin);
	AABB GetAABB();

};

