#pragma once
#include "../Core/VariableTypes.h"
#include "../Input/Input.h"


class DebugCamera {
private:
    Vector3 eye_{};
    Vector3 target_{};
    Vector3 up_{};

    Vector2i mousePrevPos_{};
    bool isMiddleMouseButtonPressed_=false;

    float radius_=0.0f;
    float phi_=0.0f;
    float theta_=0.0f;

    Matrix4x4 viewMatrix_{};

public:
    void Initialize();
    void Update();
    Matrix4x4 GetViewMatrix()const { return viewMatrix_; }
    Vector3 GetTarget() const { return target_; }
};
