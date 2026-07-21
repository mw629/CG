#pragma once
#include"ObjectBase.h"


struct ModelSubMeshMaterial {
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};
	std::unique_ptr<MaterialFactory> materialFactory;
};

class Model	:public ObjectBase
{
private:
	int modelNumber_;

	//アニメーション
	Node rootNode_;

	std::vector<ModelSubMeshMaterial> subMeshMaterials_;

public:
	
	~Model()override;


	void Initialize(ModelData modelData);

	void SettingWvp(Matrix4x4 viewMatrix) override;
	
	void CreateObject()override;

	void SetLighting(bool isActive) {
		ObjectBase::SetLighting(isActive);
		for (auto& mat : subMeshMaterials_) {
			if (mat.materialFactory) {
				mat.materialFactory->SetMaterialLighting(isActive);
			}
		}
	}

	void SetColor(Vector4 color) {
		if (auto matComp = GetComponent<MaterialComponent>()) {
			if (matComp->GetMaterialFactory()) {
				matComp->GetMaterialFactory()->SetColor(color);
			}
		}
		for (auto& mat : subMeshMaterials_) {
			if (mat.materialFactory) {
				mat.materialFactory->SetColor(color);
			}
		}
	}

	Mesh GetMesh() override;

	int GetModelNumber() { return modelNumber_; }

	const std::vector<ModelSubMeshMaterial>& GetSubMeshMaterials() const { return subMeshMaterials_; }
};

