#pragma once
#include <d3d12.h> 
#include "RenderState.h"

class RootParameter
{
private:
	D3D12_ROOT_PARAMETER rootParameter[12] = {};

	D3D12_ROOT_PARAMETER ParticleRootParameter[3] = {};
	D3D12_DESCRIPTOR_RANGE descriptorRange_[2] = {}; // changed to 2 ranges for animation (matrix palette t0 and texture t1)
	
public:

	void CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature,ShaderName shader);

	void CreateOBJRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void CreateAnimationRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature, ShaderName shader);

	void CreateLineRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

	void CreateParticleParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature);

};

