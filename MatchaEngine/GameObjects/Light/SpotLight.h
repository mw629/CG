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


	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_ = { 2.0f,1.25f,0.0f };
	float intensity_ = 7.0f;
	Vector3 direction_ = { -1.0f,-1.0f,0.0f };
	float distance_ = 4.0f;
	float decay_ = 2.0f;
	float cosAngle_ = std::cos(std::numbers::pi_v<float> / 3.0f);
	float cosFalloffStart_=cosAngle_ + 0.1f;

public:

	void ImGui();

	void CreatePointLight();

	ID3D12Resource* GetDirectinalLightResource() { return spotLightResource_.Get(); }
	SpotLightData* GetDirectinalLightData() { return spotLightData_; }


};