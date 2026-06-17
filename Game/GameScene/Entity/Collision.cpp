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
		transform.translate.y,
		transform.translate.z - depth / 2.0f
	};
	aabb.max = {
		transform.translate.x + width / 2.0f,
		transform.translate.y + height,
		transform.translate.z + depth / 2.0f
	};
	return aabb;
}

bool Collision::CheckAABBSphere(const AABB& aabb, const CollisionSphere& sphere)
{
	float sqDist = 0.0f;
	
	if (sphere.center.x < aabb.min.x) sqDist += (aabb.min.x - sphere.center.x) * (aabb.min.x - sphere.center.x);
	else if (sphere.center.x > aabb.max.x) sqDist += (sphere.center.x - aabb.max.x) * (sphere.center.x - aabb.max.x);
	
	if (sphere.center.y < aabb.min.y) sqDist += (aabb.min.y - sphere.center.y) * (aabb.min.y - sphere.center.y);
	else if (sphere.center.y > aabb.max.y) sqDist += (sphere.center.y - aabb.max.y) * (sphere.center.y - aabb.max.y);
	
	if (sphere.center.z < aabb.min.z) sqDist += (aabb.min.z - sphere.center.z) * (aabb.min.z - sphere.center.z);
	else if (sphere.center.z > aabb.max.z) sqDist += (sphere.center.z - aabb.max.z) * (sphere.center.z - aabb.max.z);
	
	return sqDist <= sphere.radius * sphere.radius;
}

CollisionSphere Collision::MakeSphere(const Transform& transform, float radius)
{
	CollisionSphere sphere;
	sphere.center = {
		transform.translate.x,
		transform.translate.y + radius,
		transform.translate.z
	};
	sphere.radius = radius;
	return sphere;
}
