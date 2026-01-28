#pragma once
#include <Engine.h>

class Player;

class TrackingCamera
{
private:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	float kVelocityBias = 50.0f;

	std::unique_ptr<Camera>camera_;
	Transform transform_;

	Player* target_ = nullptr;

	Vector3 targetVelocity_;
	Vector3 targeOffset = { 0, 0, -30.0f };

	// separate interpolation rates per axis so X can follow stronger and Y weaker
	float kInterpolationRateX = 0.01f;
	float kInterpolationRateY = 0.001f;
	float kInterpolationRateZ = 0.001f;
	Vector3 targetLocation_{};
	const float kDeathZoomFactor = 0.5f;

	Rect movableArea_ = { 0, 500, 0, 100 };

	Rect Margin = { -10.0f, 10.0f, -0.5f, 0.5f };


	bool hasPrev_;
	Vector3 prevTargetPos_;

public:

	void ImGui();

	void Initialize(Player* player);

	void Update();

	void Reset();

	void SetTargetVelocity(Vector3 velocity) { targetVelocity_ = velocity; };

	void SetMovableArea(Rect area) { movableArea_ = area; };

	Matrix4x4 GetView() { return camera_.get()->GetViewMatrix(); }


};

