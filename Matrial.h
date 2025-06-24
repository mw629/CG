#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <Common/VariableTypes.h>

class Matrial
{
private:

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Material* materialData;

public:
	
	void CreateMatrial(ID3D12Device* device);

};

