#pragma once
#include "../Core/VariableTypes.h"
#include "DebugCamera.h"
#include <d3d12.h>
#include <wrl.h>
#include "Calculation.h"

class Camera
{
private:

	Transform transform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform gameTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Matrix4x4 viewMatrix_{};
	Matrix4x4 projectionMatrix_{};

	float fovY_ = 0.45f;
	float aspectRatio_ = 1280.0f / 720.0f;
	float nearClip_ = 0.1f;
	float farClip_ = 10000.0f;


	DebugCamera debugCamera_;

	bool isDebugCamera_=true;
	float totalTime_ = 0.0f;

	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;


public:
	Camera();


	void ImGui();

	void Initialize();
	void Update();
	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }
	Matrix4x4 GetProjectionMatrix() const { return projectionMatrix_; }
	Matrix4x4 GetViewProjectionMatrix() const { return MultiplyMatrix4x4(viewMatrix_, projectionMatrix_); }

	const Transform& GetTransform() const { return transform_; }
	const Transform& GetGameTransform() const { return gameTransform_; }
	void SetTransform(Transform transform);
	void SetDebugCamera(bool isDebug) { isDebugCamera_ = isDebug; }
	bool GetDebugCamera() const { return isDebugCamera_; }
	void ResetDebugCameraToGameCamera();
	void ResetDebugCamera(const Transform& transform);
	DebugCamera& GetDebugCameraRef() { return debugCamera_; }

	void SetAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio; }
	float GetAspectRatio() const { return aspectRatio_; }
	
	ID3D12Resource* GetCameraResource() const { return cameraResource_.Get(); }

	const Frustum& GetFrustum() const { return frustum_; }

private:
	Frustum frustum_{};
};

