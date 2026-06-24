#pragma once
#include <vector>
#include <memory>

class ColliderComponent;

class CollisionManager
{
private:
	CollisionManager() = default;
	~CollisionManager() = default;

	std::vector<ColliderComponent*> colliders_;

public:
	// シングルトンインスタンスの取得
	static CollisionManager* GetInstance();

	// コピー不可
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;

	// コライダーの登録と解除
	void RegisterCollider(ColliderComponent* collider);
	void UnregisterCollider(ColliderComponent* collider);

	// 全コライダーの当たり判定をチェック
	void UpdateCollisions();

	// リセット（シーン遷移時など）
	void Clear();
};
