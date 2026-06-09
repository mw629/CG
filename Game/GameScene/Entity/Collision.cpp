#include "Collision.h"

bool Collision::CheckAABB(const AABB& a, const AABB& b)
{
	// 各軸で重なりがあるかチェック
	if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
	if (a.max.z < b.min.z || a.min.z > b.max.z) return false;

	return true;
}

AABB Collision::MakeAABB(const Transform& transform, float width, float height, float depth)
{
	AABB aabb;
	aabb.min = {
		transform.translate.x - width / 2.0f,
		transform.translate.y - height / 2.0f,
		transform.translate.z - depth / 2.0f
	};
	aabb.max = {
		transform.translate.x + width / 2.0f,
		transform.translate.y + height / 2.0f,
		transform.translate.z + depth / 2.0f
	};
	return aabb;
}
