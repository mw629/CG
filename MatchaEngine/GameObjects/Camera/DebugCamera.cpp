#include "DebugCamera.h"
#include <cmath>
#include "Math/Calculation.h"

void DebugCamera::Initialize() {
    eye_ = { 0.0f, 5.0f, -10.0f };
    target_ = { 0.0f, 0.0f, 0.0f };
    up_ = { 0.0f, 1.0f, 0.0f };

    Vector3 diff = eye_ - target_;
    radius_ = Length(diff);
    phi_ = std::atan2(diff.y, std::sqrt(diff.x * diff.x + diff.z * diff.z));
    theta_ = std::atan2(diff.x, diff.z);

    mousePrevPos_ = { 0, 0 };
    isMiddleMouseButtonPressed_ = false;
}

void DebugCamera::Update() {
    isMiddleMouseButtonPressed_ = Input::PressMouse(2);

    Vector2 mouseDelta = Input::GetMouseDelta();
    float M_PI = 3.14f;
    float moveSpeed = 0.1f;
    float rotateSpeed = 0.005f;
    float zoomSpeed = 1.0f;

    if (isMiddleMouseButtonPressed_ && !Input::PressKey(DIK_LSHIFT) && !Input::PressKey(DIK_LCONTROL)) {
        phi_ -= (float)mouseDelta.y * rotateSpeed;
        theta_ += (float)mouseDelta.x * rotateSpeed;
        phi_ = std::fmaxf(-M_PI / 2.0f + 0.01f, std::fminf(M_PI / 2.0f - 0.01f, phi_));
    }
    else if (isMiddleMouseButtonPressed_ && Input::PressKey(DIK_LSHIFT)) {
        Vector3 forward = Normalize(target_ - eye_);
        Vector3 right = Normalize(Cross(up_, forward));
        Vector3 localUp = Normalize(Cross(forward, right));

        float panFactor = radius_ * 0.001f;

        eye_ += right * ((float)-mouseDelta.x * panFactor);
        target_ += right * ((float)-mouseDelta.x * panFactor);

        eye_ += localUp * ((float)mouseDelta.y * panFactor);
        target_ += localUp * ((float)mouseDelta.y * panFactor);
    }

    float wheelDelta = (float)Input::GetMouseWheel();
    if (wheelDelta != 0) {
        radius_ -= wheelDelta * zoomSpeed * 0.1f;
        if (radius_ < 0.1f) {
            radius_ = 0.1f;
        }
    }

    if (Input::PressKey(DIK_W)) {
        Vector3 forward = Normalize(target_ - eye_);
        eye_ += forward * moveSpeed;
        target_ += forward * moveSpeed;
    }
    if (Input::PressKey(DIK_S)) {
        Vector3 forward = Normalize(target_ - eye_);
        eye_ -= forward * moveSpeed;
        target_ -= forward * moveSpeed;
    }

    eye_.x = target_.x + radius_ * std::sin(theta_) * std::cos(phi_);
    eye_.y = target_.y + radius_ * std::sin(phi_);
    eye_.z = target_.z + radius_ * std::cos(theta_) * std::cos(phi_);

    viewMatrix_ = MakeLookAtLH(eye_, target_, up_);
}
