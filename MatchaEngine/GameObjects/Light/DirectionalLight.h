#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"

class DirectionalLight
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLightData* directionalLightData_ = nullptr;

	static const int kNumLights = 4;

	Vector4 color_[kNumLights] = {
		{1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f}
	};
	Vector3 direction_[kNumLights] = {
		{0.0f,-1.0f,0.0f}, {0.0f,-1.0f,0.0f},
		{0.0f,-1.0f,0.0f}, {0.0f,-1.0f,0.0f}
	};
	float intensity_[kNumLights] = { 1.0f, 0.0f, 0.0f, 0.0f };
	bool active_[kNumLights] = { true, false, false, false };

public:

	void ImGui();

	void CreateDirectionalLight();

	ID3D12Resource* GetDirectinalLightResource() { return directionalLightResource_.Get(); }
	DirectionalLightData* GetDirectinalLightData() { return directionalLightData_; }


};

