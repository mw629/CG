#pragma once
#include <d3d12.h> 
#include "RenderState.h"

class RootParameter
{
private:
	D3D12_ROOT_PARAMETER rootParameter[12] = {};

	D3D12_ROOT_PARAMETER ParticleRootParameter[3] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};

public:

	void CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature, ShaderName shader);

	void CreateOBJRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void  CreateSpriteRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void CreateLineRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void CreateParticleParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

};

