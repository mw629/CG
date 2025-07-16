#pragma once
#include <d3d12.h> 

class RootParameter
{
private:
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	
	D3D12_ROOT_PARAMETER lineRootParameter[1] = {};

public:
	void CreateDescriptorRange();

	void CreateLineRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);
};

