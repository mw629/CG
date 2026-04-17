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

	void ImGui();

	void CreateMartial(bool Lighting = true);

	void SetColor(Vector4 color) { materialData_->color = color; }
	void SetMaterialLighting(bool isActive) { materialData_->enableLighting = isActive ? 1 : 0; }

	ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }
	Material* GetMaterialData() { return materialData_; }
};
