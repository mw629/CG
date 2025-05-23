#pragma once
#include "externals/DirectXTex/d3dx12.h"

class RootSignature
{
private:
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

public:
	void CreateRootSignature();

	D3D12_DESCRIPTOR_RANGE GetDescriptorRange(int i) { return descriptorRange[i]; };
	D3D12_ROOT_SIGNATURE_DESC GetDescriptionRootSignature() { return descriptionRootSignature; };
};

