#include "DebugCamera.h"
#include <cmath>
#include "Math/Calculation.h"

// ============================================================
// Blender-style Debug Camera Controls:
//   RMB drag           : Orbit (rotate around target)
//   Shift + RMB drag   : Pan (translate camera & target)
//   MMB drag           : Pan (same as Shift+RMB)
//   Mouse wheel        : Zoom (adjust distance to target)
//   Numpad 0           : Reset to initial position
//   Numpad 1           : Front view  (+Z axis)
//   Numpad 3           : Right view  (+X axis)
//   Numpad 7           : Top view    (+Y axis)
// ============================================================

static constexpr float k_PI         = 3.14159265f;
static constexpr float k_rotSpeed   = 0.007f;   // orbit sensitivity
static constexpr float k_panBase    = 0.0015f;   // pan factor (scales with radius)
static constexpr float k_zoomSpeed  = 0.015f;    // wheel zoom speed

void DebugCamera::Initialize() {
    eye_    = { 0.0f, 5.0f, -10.0f };
    target_ = { 0.0f, 0.0f,   0.0f };
    up_     = { 0.0f, 1.0f,   0.0f };

    Vector3 diff = eye_ - target_;
    radius_ = Length(diff);
    phi_    = std::atan2(diff.y, std::sqrt(diff.x * diff.x + diff.z * diff.z));
    theta_  = std::atan2(diff.x, diff.z);

    mousePrevPos_              = { 0, 0 };
    isRightMouseButtonPressed_ = false;
    isMiddleMouseButtonPressed_= false;
}

void DebugCamera::Update() {
    isRightMouseButtonPressed_  = Input::PressMouse(1);  // RMB
    isMiddleMouseButtonPressed_ = Input::PressMouse(2);  // MMB

    Vector2 mouseDelta = Input::GetMouseDelta();

    // ---- Orbit (RMB drag, no shift) ----
    bool wantOrbit = isRightMouseButtonPressed_ && !Input::PressKey(DIK_LSHIFT);
    if (wantOrbit) {
        theta_ += (float)mouseDelta.x * k_rotSpeed;
        phi_   -= (float)mouseDelta.y * k_rotSpeed;
        // Clamp phi to avoid gimbal flip
        phi_ = std::fmaxf(-k_PI * 0.5f + 0.01f, std::fminf(k_PI * 0.5f - 0.01f, phi_));
    }

    // ---- Pan (Shift+RMB  or  MMB) ----
    bool wantPan = (isRightMouseButtonPressed_ && Input::PressKey(DIK_LSHIFT))
                 || isMiddleMouseButtonPressed_;
    if (wantPan) {
        Vector3 forward  = Normalize(target_ - eye_);
        Vector3 right    = Normalize(Cross(up_, forward));
        Vector3 localUp  = Normalize(Cross(forward, right));

        float panFactor = radius_ * k_panBase;

        eye_    += right   * ((float)-mouseDelta.x * panFactor);
        target_ += right   * ((float)-mouseDelta.x * panFactor);
        eye_    += localUp * ((float) mouseDelta.y * panFactor);
        target_ += localUp * ((float) mouseDelta.y * panFactor);
    }

    // ---- Zoom (mouse wheel) ----
    float wheelDelta = (float)Input::GetMouseWheel();
    if (wheelDelta != 0.0f) {
        radius_ -= wheelDelta * k_zoomSpeed;
        if (radius_ < 0.1f) radius_ = 0.1f;
    }

    // ---- Numpad view snaps ----
    // Numpad 0 : reset
    if (Input::PushKey(DIK_NUMPAD0)) {
        target_ = { 0.0f, 0.0f, 0.0f };
        radius_ = Length(Vector3{ 0.0f, 5.0f, -10.0f } - target_);
        phi_    = std::atan2(5.0f, std::sqrt(0.0f * 0.0f + (-10.0f) * (-10.0f)));
        theta_  = std::atan2(0.0f, -10.0f);
    }
    // Numpad 1 : front view (look from +Z toward origin)
    if (Input::PushKey(DIK_NUMPAD1)) {
        theta_ = 0.0f;
        phi_   = 0.0f;
    }
    // Numpad 3 : right view (look from +X toward origin)
    if (Input::PushKey(DIK_NUMPAD3)) {
        theta_ = k_PI * 0.5f;
        phi_   = 0.0f;
    }
    // Numpad 7 : top view (look from +Y toward origin)
    if (Input::PushKey(DIK_NUMPAD7)) {
        theta_ = 0.0f;
        phi_   = k_PI * 0.5f - 0.01f;
    }

    // ---- Recompute eye position from spherical coords ----
    eye_.x = target_.x + radius_ * std::sin(theta_) * std::cos(phi_);
    eye_.y = target_.y + radius_ * std::sin(phi_);
    eye_.z = target_.z + radius_ * std::cos(theta_) * std::cos(phi_);

    viewMatrix_ = MakeLookAtLH(eye_, target_, up_);
}
