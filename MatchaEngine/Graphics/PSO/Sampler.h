#pragma once
#include <d3dx12.h>

class Sampler
{
private:
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
public:
	void CreateSampler(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);
};

