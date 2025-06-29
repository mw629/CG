#pragma once
#include "Common/VariableTypes.h"
#include "Common/Input.h"


class DebugCamera {
private:
    Vector3 eye_;
    Vector3 target_;
    Vector3 up_;

    Vector2i mousePrevPos_;
    bool isMiddleMouseButtonPressed_;

    float radius_;
    float phi_;
    float theta_;

    Matrix4x4 viewMatrix_;

public:
    void Initialize();
    void Update(Input* input);
    Matrix4x4 GetViewMatrix() { return viewMatrix_; }
    Vector3 GetTarget() const { return target_; }
};
