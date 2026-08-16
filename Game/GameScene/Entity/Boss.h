#pragma once
#include <Engine.h>
#include <memory>
#include "GameObject.h"

class Boss : public GameObject {
private:
  std::unique_ptr<Model> model_ = std::make_unique<Model>();
  int hp_ = 5;
  bool isHit_ = false;
  float hitTimer_ = 0.0f;

public:
  Boss();
  ~Boss();

  void Initialize(ModelData modelData);
  void Spawn(float x, float y, float z);
  
  void Update(Matrix4x4 view, float speedMultiplier = 1.0f) override;
  void Draw(class Draw &draw) override;
  void ImGuiInnerComponents() override;

  bool HasMaterial() const override {
    return (model_ && model_->GetComponent<MaterialComponent>() != nullptr) ||
           GameObject::HasMaterial();
  }


  void OnDamage();
  int GetHP() const { return hp_; }
};
