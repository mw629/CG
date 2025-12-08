#pragma once  
#include <wrl.h>  
#include <d3d12.h>  
#include "VariableTypes.h"
#include "Calculation.h"

class MaterialFactory
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_{};
	Material* materialData_{};

public:
	~MaterialFactory();

	void CreateMatrial(ID3D12Device* device,bool Lighting);

	void SetColor(Vector4 color) { materialData_->color = color; }
	void SetMaterialLighting(bool isActiv) { materialData_->endbleLighting = isActiv; }
	void SetUVTransform(Transform transform) { materialData_->uvTransform = MakeAffineMatrix(transform); }

	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	Material* GetMaterialData() { return materialData_; }
};
