#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"
#include <numbers>

class SpotLight
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
	SpotLightData* spotLightData_ = nullptr;

	static const int kNumLights = 4;

	Vector4 color_[kNumLights] = {
		{1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f,1.0f}, {1.0f,1.0f,1.0f,1.0f}
	};
	Vector3 position_[kNumLights] = {
		{2.0f,1.25f,0.0f}, {2.0f,1.25f,0.0f},
		{2.0f,1.25f,0.0f}, {2.0f,1.25f,0.0f}
	};
	float intensity_[kNumLights] = { 7.0f, 0.0f, 0.0f, 0.0f };
	Vector3 direction_[kNumLights] = {
		{-1.0f,-1.0f,0.0f}, {-1.0f,-1.0f,0.0f},
		{-1.0f,-1.0f,0.0f}, {-1.0f,-1.0f,0.0f}
	};
	float distance_[kNumLights] = { 4.0f, 4.0f, 4.0f, 4.0f };
	float decay_[kNumLights] = { 2.0f, 2.0f, 2.0f, 2.0f };
	float cosAngle_[kNumLights] = {
		std::cos(std::numbers::pi_v<float> / 3.0f), std::cos(std::numbers::pi_v<float> / 3.0f),
		std::cos(std::numbers::pi_v<float> / 3.0f), std::cos(std::numbers::pi_v<float> / 3.0f)
	};
	float cosFalloffStart_[kNumLights] = {
		cosAngle_[0] + 0.1f, cosAngle_[1] + 0.1f,
		cosAngle_[2] + 0.1f, cosAngle_[3] + 0.1f
	};
	bool active_[kNumLights] = { true, false, false, false };

public:

	void ImGui();

	void CreatePointLight();

	ID3D12Resource* GetDirectinalLightResource() { return spotLightResource_.Get(); }
	SpotLightData* GetDirectinalLightData() { return spotLightData_; }


};