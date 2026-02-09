#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"

class PointLight
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
	PointLightData*pointLightData_ = nullptr;


	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_ = { 0.0f,2.0f,0.0f };
	float intensity_ = 1.0f;
	float radius_ = 10.0f;
	float decay_ = 2.0f;

public:

	void ImGui();

	void CreatePointLight();

	ID3D12Resource* GetDirectinalLightResource() { return pointLightResource_.Get(); }
	PointLightData* GetDirectinalLightData() { return pointLightData_; }


};

