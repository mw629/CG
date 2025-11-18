#pragma once
#include <d3d12.h> 
#include "RenderState.h"

class RootParameter
{
private:
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	
	D3D12_ROOT_PARAMETER lineRootParameter[1] = {};

public:
	void CreateDescriptorRange();

	void CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature,ShaderName shader);

	void CreateLineRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void CreateParticleParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void CreateOBJRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);
};

