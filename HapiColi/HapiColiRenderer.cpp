#include "HapiColiRenderer.h"
#include "../MatchaEngine/Math/Calculation.h"

namespace HapiColi {

void HapiColiRenderer::ClearCommands() {
    commands_.clear();
}

void HapiColiRenderer::BuildCommands(const std::vector<ObjectData>& objects) {
    ClearCommands();
    if (objects.empty()) return;

    for (const auto& obj : objects) {
        if (obj.collider.type != ColliderInfo::Type::Box) continue;

        ::Vector3 pos = { obj.position.x, obj.position.y, obj.position.z };
        ::Vector3 scale = { obj.collider.size.x, obj.collider.size.y, obj.collider.size.z };
        ::Quaternion rot = { obj.rotation.x, obj.rotation.y, obj.rotation.z, obj.rotation.w };

        float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Green by default
        if (obj.collision.isColliding) {
            color[0] = 1.0f; color[1] = 0.0f; color[2] = 0.0f; // Red if colliding
        }

        AddBoxLines(pos, scale, rot, color);
    }
}

const std::vector<RenderCommand>& HapiColiRenderer::GetCommands() const {
    return commands_;
}

void HapiColiRenderer::AddBoxLines(const ::Vector3& pos, const ::Vector3& scale, const ::Quaternion& rot, float color[4]) {
    Matrix4x4 world = MakeAffineMatrix(pos, scale, rot);
    
    ::Vector3 corners[8] = {
        {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
        { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},
        {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f},
        { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}
    };

    for (int i = 0; i < 8; ++i) {
        corners[i] = TransformMatrix(corners[i], world);
    }

    // Bottom face
    commands_.push_back({ corners[0], corners[1], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[1], corners[5], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[5], corners[4], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[4], corners[0], {color[0], color[1], color[2], color[3]} });

    // Top face
    commands_.push_back({ corners[3], corners[2], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[2], corners[6], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[6], corners[7], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[7], corners[3], {color[0], color[1], color[2], color[3]} });

    // Vertical lines
    commands_.push_back({ corners[0], corners[3], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[1], corners[2], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[4], corners[7], {color[0], color[1], color[2], color[3]} });
    commands_.push_back({ corners[5], corners[6], {color[0], color[1], color[2], color[3]} });
}

}
