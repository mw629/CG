#pragma once
#include "GameObject.h"
#include <memory>
#include "Graphics/Render/Draw.h"

class RenderObject : public GameObject {
private:
    std::shared_ptr<ObjectBase> objectBase_;

public:
    RenderObject() {}
    RenderObject(std::shared_ptr<ObjectBase> obj) : objectBase_(obj) {
        if (objectBase_) name_ = objectBase_->name_;
    }

    void SetObjectBase(std::shared_ptr<ObjectBase> obj) {
        objectBase_ = obj;
        if (objectBase_) name_ = objectBase_->name_;
    }

    void Update(Matrix4x4 view, float speedMultiplier = 1.0f) override {
        if (objectBase_) {
            objectBase_->SetTransform(transform_);
            objectBase_->SettingWvp(view);
        }
    }

    void Draw() override {
        if (objectBase_) {
            Draw::DrawObj(objectBase_.get());
        }
    }

    void SetTransform(const Transform& transform) override {
        transform_ = transform;
        if (objectBase_) {
            objectBase_->SetTransform(transform_);
        }
    }

    void ImGui() override {
        GameObject::ImGui();
        if (objectBase_) {
            objectBase_->ImGui();
        }
    }

    std::shared_ptr<ObjectBase> GetObjectBase() const { return objectBase_; }
};
