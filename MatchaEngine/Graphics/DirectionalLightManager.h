#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "Core/VariableTypes.h"

class DirectionalLightManager
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource_;
	DirectionalLight* directinalLightData_ = nullptr;

	Vector4 color_		= { 1.0f,1.0f,1.0f,1.0f };
	Vector3 direction_	= { 0.0f,-1.0f,0.0f };
	float intensity_	=	1.0f;

public:
	void CreateDirectinalLight(ID3D12Device *device);

	ID3D12Resource* GetDirectinalLightResource() { return directinalLightResource_.Get(); }
	DirectionalLight* GetDirectinalLightData() { return directinalLightData_; }

	void SetDirectionalLight(DirectionalLight light) { 
		directinalLightData_->color = light.color;
		directinalLightData_->direction = light.direction;
		directinalLightData_->intensity = light.intensity;
	}
};

