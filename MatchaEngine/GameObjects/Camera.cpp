#include "Camera.h"
#include <Engine.h>

void Camera::Initialize() {
	viewMatrix_ = Inverse(MakeAffineMatrix(transform_.translate,transform_.scale,transform_.rotate));
}

void Camera::Update() {
	viewMatrix_ = Inverse(MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate));
}


