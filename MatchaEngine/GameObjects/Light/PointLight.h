#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"

class PointLight
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	PointLightData* pointLightData_ = nullptr;

	static const int kNumLights = 4;

	Vector4 color_[kNumLights] = {
		{1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f}
	};
	Vector3 position_[kNumLights] = {
		{0.0f,2.0f,0.0f}, {0.0f,2.0f,0.0f},
		{0.0f,2.0f,0.0f}, {0.0f,2.0f,0.0f}
	};
	float intensity_[kNumLights] = { 1.0f, 0.0f, 0.0f, 0.0f };
	float radius_[kNumLights] = { 10.0f, 10.0f, 10.0f, 10.0f };
	float decay_[kNumLights] = { 2.0f, 2.0f, 2.0f, 2.0f };
	bool active_[kNumLights] = { true, false, false, false };

public:

	void ImGui();

	void CreatePointLight();

	ID3D12Resource* GetDirectinalLightResource() { return pointLightResource_.Get(); }
	PointLightData* GetDirectinalLightData() { return pointLightData_; }


};

