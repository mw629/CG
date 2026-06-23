#pragma once
#include <string>
#include <memory>
#include "ObjectBase.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif

class GameObject
{
protected:
    std::string name_ = "GameObject";
    Transform transform_{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
    bool isActive_ = true;

public:
    GameObject() {}
    virtual ~GameObject() = default;

    virtual void Initialize() {}
    virtual void Update(Matrix4x4 view, float speedMultiplier = 1.0f) {}
    virtual void Draw() {}
    virtual void ImGui() {
#ifdef _USE_IMGUI
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            float t[3] = { transform_.translate.x, transform_.translate.y, transform_.translate.z };
            if (ImGui::DragFloat3("Position", t, 0.1f)) {
                transform_.translate = { t[0], t[1], t[2] };
                SetTransform(transform_);
            }
            float r[3] = { transform_.rotate.x, transform_.rotate.y, transform_.rotate.z };
            if (ImGui::DragFloat3("Rotation", r, 0.1f)) {
                transform_.rotate = { r[0], r[1], r[2] };
                SetTransform(transform_);
            }
            float s[3] = { transform_.scale.x, transform_.scale.y, transform_.scale.z };
            if (ImGui::DragFloat3("Scale", s, 0.1f)) {
                transform_.scale = { s[0], s[1], s[2] };
                SetTransform(transform_);
            }
        }
#endif
    }

    const std::string& GetName() const { return name_; }
    void SetName(const std::string& name) { name_ = name; }

    const Transform& GetTransform() const { return transform_; }
    virtual void SetTransform(const Transform& transform) { transform_ = transform; }

    bool GetIsActive() const { return isActive_; }
    void SetIsActive(bool active) { isActive_ = active; }
};
