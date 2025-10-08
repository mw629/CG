#include "Entity.h"

AABB Entity::GetAABB()
{
	Vector3 worldPos = transform_.translate;
	AABB aabb;
	aabb.min = { worldPos.x - kWidth_ / 2.0f, worldPos.y - kHeight_ / 2.0f, worldPos.z - kWidth_ / 2.0f };
	aabb.max = { worldPos.x + kWidth_ / 2.0f, worldPos.y + kHeight_ / 2.0f, worldPos.z + kWidth_ / 2.0f };

	return aabb;
}
