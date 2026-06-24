#pragma once
#include "GameObject.h"
#include <memory>
#include "Graphics/Render/Draw.h"
#include "Model.h"

class RenderObject : public GameObject {
private:
    std::shared_ptr<ObjectBase> objectBase_;
public:
    std::string modelFilePath_ = "";

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
        GameObject::Update(view, speedMultiplier);
        if (objectBase_) {
            objectBase_->SetTransform(transform_);
            objectBase_->SettingWvp(view);
        }
    }

    void Draw() override {
        GameObject::Draw();
        if (objectBase_) {
            if (auto model = dynamic_cast<Model*>(objectBase_.get())) {
                Draw::DrawModel(model);
            } else {
                Draw::DrawObj(objectBase_.get());
            }
        }
    }

    void SetTransform(const Transform& transform) override {
        transform_ = transform;
        if (objectBase_) {
            objectBase_->SetTransform(transform_);
        }
    }

    void ImGuiInnerComponents() override {
        if (objectBase_) {
            objectBase_->ImGui(false);
        }
    }

    bool HasMaterial() const override {
        return objectBase_ && objectBase_->GetComponent<MaterialComponent>() != nullptr || GameObject::HasMaterial();
    }

    std::shared_ptr<ObjectBase> GetObjectBase() const { return objectBase_; }
};
