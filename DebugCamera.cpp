#include "DebugCamera.h"
#include "MatchaEngine/Math/Calculation.h"

void DebugCamera::Initialize()
{
	translation_ = { 0,0,-50 };
	rotation_ = { 0,0,0 };
	camera_ = { {1.0f,1.0f,1.0f},{0,0,0},{0,0,-10.0f} };
	cameraMatrix_ = MakeAffineMatrix(camera_.translate, camera_.scale, camera_.rotate);
	viewMatrix_ = Inverse(cameraMatrix_);
	ProjectionMatrix_ = MakePerspectiveFovMatrix(fovY, aspect, zn, zf);
}

void DebugCamera::CameraMove(Input input)
{
    // 回転入力
    if (input.PressKey(DIK_LEFT)) {
        camera_.rotate.y -= angle;
    }
    if (input.PressKey(DIK_RIGHT)) {
        camera_.rotate.y += angle;
    }
    if (input.PressKey(DIK_DOWN)) {
        camera_.rotate.x -= angle;
    }
    if (input.PressKey(DIK_UP)) {
        camera_.rotate.x += angle;
    }
    if (input.PressKey(DIK_Q)) {
        camera_.rotate.z -= angle;
    }
    if (input.PressKey(DIK_E)) {
        camera_.rotate.z += angle;
    }

    // 毎フレーム再計算するように修正
    matRot_ = RotationX(camera_.rotate.x) * RotationY(camera_.rotate.y);

    // 移動入力
    Vector3 move = { 0, 0, 0 };
    if (input.PressKey(DIK_LSHIFT)) {
        if (input.PressKey(DIK_W)) {
            move.z += speed;
        }
        if (input.PressKey(DIK_S)) {
            move.z -= speed;
        }
    }
    else {
        if (input.PressKey(DIK_W)) {
            move.y += speed;
        }
        if (input.PressKey(DIK_S)) {
            move.y -= speed;
        }
    }
    if (input.PressKey(DIK_A)) {
        move.x -= speed;
    }
    if (input.PressKey(DIK_D)) {
        move.x += speed;
    }

    // 回転を適用してから移動
    move = TransformMatrix(move, matRot_);
    camera_.translate += move;
}

void DebugCamera::Update(Input input)
{
	CameraMove(input);
	cameraMatrix_ = MakeAffineMatrix(camera_.translate, camera_.scale, camera_.rotate);
	viewMatrix_ = Inverse(cameraMatrix_);
}
