#pragma once
#include <Engine.h>
#include "../Map/MapChipField.h"

class Entity
{
protected:

#pragma region Struct
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceilingCollisionFlag = false; // 天井衝突フラグ
		bool landingFlag = false;          // 着地フラグ
		bool wallContactFlag = false;      // 壁接触フラグ
		Vector3 velocity;                  // 移動量
	};
#pragma endregion
#pragma region Enum
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上
		kNumCorner    // 要素数
	};

	enum class LRDirection {
		kRight,
		kLeft,
	};
#pragma endregion

#pragma region 構成のデータ
	std::unique_ptr<Model> model_;
	Transform transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };
	
	Vector4 color_ = { 1.0f,0.0f,0.0f,1.0f };
	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;
	// 向き
	LRDirection lrDirection_ = LRDirection::kRight;

	bool isActive_;
	// 設置状態フラグ
	bool onGround_ = true;
	// 接地フラグ
	bool landing = false;

#pragma endregion

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

public:
	// 指定した角の座標を得る
	Vector3 CornerPosition(const Vector3& center, Corner corner);

#pragma region 当たり判定
	// マップ衝突判定
	void MapCollision();

	virtual void HitCeiling() {}
	virtual void NotHitCeiling() {}
	virtual void OnGround() {}
	virtual void NotOnGround() {}
	virtual void HitWall() {}
	virtual void NotHitWall() {}

	void MapCollisionDetection(CollisionMapInfo& info);
	void MapCollisionDetectionUp(CollisionMapInfo& info);
	void MapCollisionDetectionDown(CollisionMapInfo& info);
	void MapCollisionDetectionLeft(CollisionMapInfo& info);
	void MapCollisionDetectionRight(CollisionMapInfo& info);

	// 天井に接触しているときの処理
	void touchingTheCeiling(const CollisionMapInfo& info);
	// 設置状態の切り替え処理
	void SwitchingInstallationStatus(const CollisionMapInfo& info);
	// 壁に接触している場合の処理
	void contactWithAWall(const CollisionMapInfo& info);
#pragma endregion
	
	AABB GetAABB();


#pragma region ゲッター
	Transform& GetTransform() { return transform_; };
	LRDirection GetDirection() { return lrDirection_; }
#pragma endregion

#pragma region セッター
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };
#pragma endregion
};

