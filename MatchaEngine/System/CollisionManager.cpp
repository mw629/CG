#include "CollisionManager.h"
#include "../GameObjects/Component/ColliderComponent.h"
#include "../GameObjects/Object/GameObject.h"
#include "../../Game/GameScene/Entity/Collision.h"
#include <algorithm>

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::RegisterCollider(ColliderComponent* collider)
{
	if (!collider) return;
	// 重複登録防止
	if (std::find(colliders_.begin(), colliders_.end(), collider) == colliders_.end()) {
		colliders_.push_back(collider);
	}
}

void CollisionManager::UnregisterCollider(ColliderComponent* collider)
{
	colliders_.erase(std::remove(colliders_.begin(), colliders_.end(), collider), colliders_.end());
}

void CollisionManager::UpdateCollisions()
{
	// 登録されている全コライダーで総当り判定
	// i と j で重複チェックを避けるため、j は i + 1 から始める
	for (size_t i = 0; i < colliders_.size(); ++i) {
		for (size_t j = i + 1; j < colliders_.size(); ++j) {
			ColliderComponent* colA = colliders_[i];
			ColliderComponent* colB = colliders_[j];

			// 親オブジェクトが非アクティブ等の場合はスキップ
			if (!colA->GetGameObject() || !colA->GetGameObject()->GetIsActive()) continue;
			if (!colB->GetGameObject() || !colB->GetGameObject()->GetIsActive()) continue;

			bool hit = false;
			ColliderShape shapeA = colA->GetShape();
			ColliderShape shapeB = colB->GetShape();

			if (shapeA == ColliderShape::Box && shapeB == ColliderShape::Box) {
				hit = Collision::CheckAABB(colA->GetWorldAABB(), colB->GetWorldAABB());
			}
			else if (shapeA == ColliderShape::Sphere && shapeB == ColliderShape::Sphere) {
				hit = Collision::CheckSphere(colA->GetWorldSphere(), colB->GetWorldSphere());
			}
			else if (shapeA == ColliderShape::Box && shapeB == ColliderShape::Sphere) {
				hit = Collision::CheckAABBSphere(colA->GetWorldAABB(), colB->GetWorldSphere());
			}
			else if (shapeA == ColliderShape::Sphere && shapeB == ColliderShape::Box) {
				hit = Collision::CheckAABBSphere(colB->GetWorldAABB(), colA->GetWorldSphere());
			}

			if (hit) {
				// 衝突発生時、それぞれの親オブジェクトに通知
				colA->GetGameObject()->OnCollision(colB->GetGameObject());
				colB->GetGameObject()->OnCollision(colA->GetGameObject());
			}
		}
	}
}

void CollisionManager::Clear()
{
	colliders_.clear();
}
