#pragma once  
#include <wrl.h>  
#include <d3d12.h>  
#include "../Core/VariableTypes.h"

class MaterialFactory
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_{};
	Material* materialData_{};

public:
	~MaterialFactory();

	void CreateMatrial(ID3D12Device* device,bool Lighting);

	void SetMaterialLighting(bool isActiv) { materialData_->endbleLighting = isActiv; }

	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	Material* GetMaterialData() { return materialData_; }
};
