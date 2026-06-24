#pragma once
#include <memory>
#include <Engine.h>

#include "GameObject.h"

/// <summary>
/// 障害物を表すクラス
/// ステージ上に配置され、手前に向かってスクロールする
/// </summary>
class Obstacle : public GameObject
{
public:
	enum class Type
	{
		Low,   // ジャンプで避ける（低い障害物）
		High,  // 転がりで避ける（高い障害物・バー）
		Wall,  // レーン移動で避ける（壁）
		Bonus  // 当たると吹き飛ぶボーナスエネミー
	};

private:
	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	ModelData normalModelData_;
	ModelData bonusModelData_;
	Type type_ = Type::Wall;

	bool isHit_ = false;
	Vector3 velocity_{ 0.0f, 0.0f, 0.0f };
	float gravity_ = 0.015f;

	// 当たり判定のサイズ
	float collisionWidth_ = 1.0f;
	float collisionHeight_ = 1.0f;
	float collisionDepth_ = 1.0f;

public:
	Obstacle();
	~Obstacle();

	void Initialize(ModelData normalData, ModelData bonusData, Type type);
	void SetType(Type type);

	/// <summary>
	/// 障害物を指定位置に配置しアクティブにする
	/// </summary>
	void Spawn(float x, float y, float z);

	/// <summary>
	/// 毎フレームの更新（手前にスクロール）
	/// </summary>
	void StageUpdate(Matrix4x4 view, float scrollSpeed);

	/// <summary>
	/// プレイヤーに当たった時の処理
	/// </summary>
	void OnHit();
	
	// Override standard Update and Draw to avoid GameObjectManager from automatically updating scroll/drawing without context
	void Update(Matrix4x4 view, float speedMultiplier = 1.0f) override {}
	void Draw() override;
	void ImGuiInnerComponents() override;

	bool HasMaterial() const override {
		return model_ && model_->GetComponent<MaterialComponent>() != nullptr || GameObject::HasMaterial();
	}

	/// <summary>
	/// 画面の手前を過ぎたら非アクティブにする
	/// </summary>
	void Deactivate() { isActive_ = false; }

	// ゲッター
	Type GetType() const { return type_; }
	float GetCollisionWidth() const { return collisionWidth_; }
	float GetCollisionHeight() const { return collisionHeight_; }
	float GetCollisionDepth() const { return collisionDepth_; }
};
