#include "HapiColiRenderer.h"
#include "../MatchaEngine/Math/Calculation.h"
#include "HapiColi.h"

namespace HapiColi {

void HapiColiRenderer::ClearCommands() {
    commands_.clear();
    solidCommands_.clear();
}

void HapiColiRenderer::BuildCommands(const std::vector<ObjectData>& objects) {
    ClearCommands();
    if (objects.empty()) return;

    for (const auto& obj : objects) {
        if (obj.collider.type != ColliderInfo::Type::Box) continue;

        ::Vector3 pos = { obj.position.x, obj.position.y, obj.position.z };
        ::Vector3 scale = { obj.collider.size.x, obj.collider.size.y, obj.collider.size.z };
        
        // 球の場合は、半径(x)の2倍を各辺とするBox（球を覆う四角）を描画する
        if (obj.collider.type == ColliderInfo::Type::Sphere) {
            scale = { obj.collider.size.x * 2.0f, obj.collider.size.x * 2.0f, obj.collider.size.x * 2.0f };
        }

        ::Quaternion rot = { obj.rotation.x, obj.rotation.y, obj.rotation.z, obj.rotation.w };

        float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f }; // Green by default
        if (obj.collision.isColliding) {
            color[0] = 1.0f; color[1] = 0.0f; color[2] = 0.0f; // Red if colliding
        }

        if (obj.collider.type == ColliderInfo::Type::Box) {
            AddBoxLines(pos, scale, rot, color);
        } else if (obj.collider.type == ColliderInfo::Type::Sphere) {
            AddSphereLines(pos, obj.collider.size.x, color);
        }

        bool shouldDrawContact = false;
        const char* activeTargetId = HapiColi::GetInstance().GetActiveTargetId();
        if (activeTargetId && std::string(obj.id) == activeTargetId) {
            shouldDrawContact = true;
        }

        if (shouldDrawContact && obj.collision.hasContactInfo) {
            ::Vector3 cp = { obj.collision.contactPoint.x, obj.collision.contactPoint.y, obj.collision.contactPoint.z };
            ::Vector3 cn = { obj.collision.contactNormal.x, obj.collision.contactNormal.y, obj.collision.contactNormal.z };
            
            float yellow[4] = {1, 1, 0, 1};
            float cyan[4] = {0, 1, 1, 1};
            
            if (cn.x != 0 || cn.y != 0 || cn.z != 0) {
                AddSolidSphere(cp, 0.15f, yellow);
                
                ::Vector3 boxRot = {0,0,0};
                if (cn.x > 0.5f) {
                    boxRot.z = -1.570796f;
                } else if (cn.x < -0.5f) {
                    boxRot.z = 1.570796f;
                } else if (cn.y > 0.5f) {
                    boxRot = {0, 0, 0};
                } else if (cn.y < -0.5f) {
                    boxRot.x = 3.14159265f;
                } else if (cn.z > 0.5f) {
                    boxRot.x = 1.570796f;
                } else if (cn.z < -0.5f) {
                    boxRot.x = -1.570796f;
                }

                // 法線の描画を見やすくするため、Boxを少し大きく、長さを2.0に
                ::Vector3 boxCenter = { cp.x + cn.x * 1.0f, cp.y + cn.y * 1.0f, cp.z + cn.z * 1.0f };
                ::Vector3 boxScale = { 0.15f, 2.0f, 0.15f };
                AddSolidBox(boxCenter, boxScale, boxRot, cyan);
            }
        }
    }
}

const std::vector<RenderCommand>& HapiColiRenderer::GetCommands() const {
    return commands_;
}

void HapiColiRenderer::AddBoxLines(const ::Vector3& pos, const ::Vector3& scale, const ::Quaternion& rot, float color[4]) {
    Matrix4x4 world = MakeAffineMatrix(pos, scale, rot);
    
    ::Vector3 corners[8] = {
        {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}
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

void HapiColiRenderer::AddSphereLines(const ::Vector3& center, float radius, float color[4]) {
    const int segments = 16;
    const float PI = 3.14159265f;
    
    // Draw 3 orthogonal circles
    for (int i = 0; i < segments; ++i) {
        float t1 = (float)i / segments * 2.0f * PI;
        float t2 = (float)(i + 1) / segments * 2.0f * PI;
        
        // XY plane
        commands_.push_back({
            {center.x + radius * cosf(t1), center.y + radius * sinf(t1), center.z},
            {center.x + radius * cosf(t2), center.y + radius * sinf(t2), center.z},
            {color[0], color[1], color[2], color[3]}
        });
        
        // YZ plane
        commands_.push_back({
            {center.x, center.y + radius * cosf(t1), center.z + radius * sinf(t1)},
            {center.x, center.y + radius * cosf(t2), center.z + radius * sinf(t2)},
            {color[0], color[1], color[2], color[3]}
        });
        
        // ZX plane
        commands_.push_back({
            {center.x + radius * sinf(t1), center.y, center.z + radius * cosf(t1)},
            {center.x + radius * sinf(t2), center.y, center.z + radius * cosf(t2)},
            {color[0], color[1], color[2], color[3]}
        });
    }
}

const std::vector<SolidRenderCommand>& HapiColiRenderer::GetSolidRenderCommands() const {
    return solidCommands_;
}

void HapiColiRenderer::AddSolidSphere(const ::Vector3& center, float radius, float color[4]) {
    solidCommands_.push_back({SolidRenderCommand::Sphere, center, {radius, radius, radius}, {0,0,0}, {color[0], color[1], color[2], color[3]}});
}

void HapiColiRenderer::AddSolidBox(const ::Vector3& center, const ::Vector3& scale, const ::Vector3& rotation, float color[4]) {
    solidCommands_.push_back({SolidRenderCommand::Box, center, scale, rotation, {color[0], color[1], color[2], color[3]}});
}

}
