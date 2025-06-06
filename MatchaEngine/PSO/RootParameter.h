#pragma once
#include "../externals/DirectXTex/d3dx12.h"
class RootParameter
{
private:
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	
public:
	void CreateDescriptorRange();

	void CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature);
};

