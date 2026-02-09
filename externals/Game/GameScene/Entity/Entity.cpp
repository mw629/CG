#include "Entity.h"

AABB Entity::GetAABB()
{
    AABB aabb{};
    
    // transformの位置とサイズからAABBを計算
    Vector3 halfSize = { kWidth_ * 0.5f, kHeight_ * 0.5f, 0.0f };
    
    aabb.min = {
        transform_.translate.x - halfSize.x,
        transform_.translate.y - halfSize.y,
        transform_.translate.z - halfSize.z
    };
    
    aabb.max = {
        transform_.translate.x + halfSize.x,
        transform_.translate.y + halfSize.y,
        transform_.translate.z + halfSize.z
    };
    
    return aabb;
}