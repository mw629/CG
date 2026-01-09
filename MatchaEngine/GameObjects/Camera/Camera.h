#pragma once
#include "../Core/VariableTypes.h"
#include "DebugCamera.h"
#include <d3d12.h>
#include <wrl.h>

class Camera
{
private:

	Transform transform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Matrix4x4 viewMatrix_{};

	DebugCamera debugCamera_;

	bool isDebugCamera_=true;

	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;


public:

	void ImGui();

	void Initialize();
	void Update();
	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }

	void SetTransform(Transform transform) { transform_ = transform; }
	
	ID3D12Resource* GetCameraResource() const { return cameraResource_.Get(); }

};

