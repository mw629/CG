#pragma once
#include <memory>
#include <Engine.h>

/// <summary>
/// 障害物を表すクラス
/// ステージ上に配置され、手前に向かってスクロールする
/// </summary>
class Obstacle
{
public:
	enum class Type
	{
		Low,   // ジャンプで避ける（低い障害物）
		High,  // 転がりで避ける（高い障害物・バー）
		Wall   // レーン移動で避ける（壁）
	};

private:
	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	Transform transform_;
	Type type_ = Type::Wall;
	bool isActive_ = false;

	// 当たり判定のサイズ
	float collisionWidth_ = 1.0f;
	float collisionHeight_ = 1.0f;
	float collisionDepth_ = 1.0f;

public:
	Obstacle();
	~Obstacle();

	void Initialize(ModelData modelData, Type type);
	void SetType(Type type);

	/// <summary>
	/// 障害物を指定位置に配置しアクティブにする
	/// </summary>
	void Spawn(float x, float y, float z);

	/// <summary>
	/// 毎フレームの更新（手前にスクロール）
	/// </summary>
	void Update(Matrix4x4 view, float scrollSpeed);

	void Draw();

	/// <summary>
	/// 画面の手前を過ぎたら非アクティブにする
	/// </summary>
	void Deactivate() { isActive_ = false; }

	// ゲッター
	bool GetIsActive() const { return isActive_; }
	const Transform& GetTransform() const { return transform_; }
	Type GetType() const { return type_; }
	float GetCollisionWidth() const { return collisionWidth_; }
	float GetCollisionHeight() const { return collisionHeight_; }
	float GetCollisionDepth() const { return collisionDepth_; }
};
