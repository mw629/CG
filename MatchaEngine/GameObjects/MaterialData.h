#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <Common/VariableTypes.h>

class MaterialData
{
private:

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_;

public:
	
	void CreateMaterial(ID3D12Device* device, bool Lighting);

	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	Material* GetMaterialData() { return materialData_; }

};


