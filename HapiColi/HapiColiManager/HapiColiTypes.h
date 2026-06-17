#pragma once

#include <string>
#include <vector>

namespace HapiColi
{
    enum class Language
    {
        English,
        Japanese
    };

    struct Vector3
    {
        float x, y, z;
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

    struct Quaternion
    {
        float x, y, z, w;
        Quaternion() : x(0), y(0), z(0), w(1) {}
        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };

    struct ColliderInfo
    {
        enum class Type
        {
            Box,
            Sphere,
            Capsule,
            Mesh,
            None
        };

        Type type = Type::None;
        Vector3 size; // Sphere: x=radius, Capsule: x=radius, y=height, Box: half-extents
        Vector3 offset;
    };

    struct CollisionResult
    {
        bool isColliding = false;
        std::string collidedWithId;
        Vector3 contactPoint;
        Vector3 contactNormal;
        bool hasContactInfo = false;
    };

    struct ObjectData
    {
        std::string id;
        std::string type;
        Vector3 position;
        Quaternion rotation;
        Vector3 velocity;
        ColliderInfo collider;
        CollisionResult collision;

        // 簡単な初期化用ヘルパー（Box）
        static ObjectData CreateBox(const std::string& id, const Vector3& position, const Vector3& size)
        {
            ObjectData data;
            data.id = id;
            data.position = position;
            data.collider.type = ColliderInfo::Type::Box;
            data.collider.size = size;
            data.collision.isColliding = false;
            return data;
        }

        // 衝突状態をセットするヘルパー
        void SetCollision(const std::string& collidedWithId)
        {
            collision.isColliding = true;
            collision.collidedWithId = collidedWithId;
        }

        // 衝突判定はないが、最後の衝突点情報を残すためのヘルパー
        void SetContactInfoOnly(const Vector3& contactPoint, const Vector3& contactNormal)
        {
            collision.contactPoint = contactPoint;
            collision.contactNormal = contactNormal;
            collision.hasContactInfo = true;
        }
    };
}
