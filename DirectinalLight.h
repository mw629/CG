#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "Common/VariableTypes.h"

class DirectinalLight
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource_;
	DirectionalLight* directinalLightData_ = nullptr;

	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 direction = { 0.0f,-1.0f,0.0f };
	float intensity=1.0f;

public:
	void CreateDirectinalLight(ID3D12Device *device);

	ID3D12Resource* GetDirectinalLightResource() { return directinalLightResource_.Get(); }
	DirectionalLight* GetDirectinalLightData() { return directinalLightData_; }

};

