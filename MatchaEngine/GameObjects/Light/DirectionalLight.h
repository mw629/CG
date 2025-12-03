#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"

class DirectionalLight
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLightData* directionalLightData_ = nullptr;


	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 direction = { 0.0f,-1.0f,0.0f };
	float intensity=1.0f;

public:

	void ImGui();

	void CreateDirectionalLight(ID3D12Device* device);

	ID3D12Resource* GetDirectinalLightResource() { return directionalLightResource_.Get(); }
	DirectionalLightData* GetDirectinalLightData() { return directionalLightData_; }


};

