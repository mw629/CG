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

	void SetColor(Vector4 color) { materialData_->color = color; }
	void SetColor255(Vector4 color) { materialData_->color = Vector4{ color.x / 255.0f,color.y / 255.0f,color.z/255.0f,color.w }; }
	void SetMaterialLighting(bool isActiv) { materialData_->endbleLighting = isActiv; }

	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	Material* GetMaterialData() { return materialData_; }
};
