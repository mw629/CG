#pragma once

#include <string>
#include <vector>

namespace HapiColi
{
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
    };
}
