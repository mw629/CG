#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"
#include "Texture.h"
#include "MaterialFactory.h"
#include "PipelineState.h"
#include "GameObject.h"
#include "../Component/MaterialComponent.h"

class ObjectBase : public GameObject
{
protected:
	// 派生クラスからアクセス可能な静的メンバー
	static float kClientWidth_;
	static float kClientHeight_;

	//objectResource

	//マテリアルデータ
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};

	//頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpDataResource_;
	TransformationMatrix* wvpData_ = nullptr;
	int vertexSize_ = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_ = nullptr;
	int indexSize_;

public:
	virtual ~ObjectBase();


	static void SetObjectResource(Vector2 ClientSize);

	virtual void CreateVertexData();
	virtual void CreateWVP();
	virtual void CreateIndexResource();

	virtual void CreateObject();

	virtual void SettingWvp(Matrix4x4 viewMatrix);


	void SetTransform(Transform transform) { transform_ = transform; }  // バグ修正: transform_ = transform_ → transform_ = transform
	void SetLighting(bool isActive) { 
		auto matComp = GetComponent<MaterialComponent>();
		if(matComp) matComp->GetMaterialFactory()->SetMaterialLighting(isActive);
	}
	void SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) { textureSrvHandleGPU_ = textureSrvHandleGPU; }

	//getter
	Transform GetTransform() const { return transform_; }
	MaterialFactory* GetMartial() { 
		auto matComp = GetComponent<MaterialComponent>();
		return matComp ? matComp->GetMaterialFactory() : nullptr;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { 
		auto matComp = GetComponent<MaterialComponent>();
		if (matComp && matComp->GetTextureSrvHandleGPU().ptr != 0) {
			return matComp->GetTextureSrvHandleGPU();
		}
		return textureSrvHandleGPU_; 
	}

	virtual Mesh GetMesh();

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView();
	ID3D12Resource* GetWvpDataResource() { return wvpDataResource_.Get(); }
	int GetVertexSize() { return vertexSize_; }
	TransformationMatrix* GetWvpData() { return wvpData_; }

	ID3D12Resource* GetIndexResource() { return indexResource_.Get(); }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }

	virtual int GetIndexSize() { return indexSize_; }

	void SetShader(ShaderName shader) { 
		if (auto mat = GetComponent<MaterialComponent>()) mat->SetShader(shader); 
	}
	void SetBlend(BlendMode blend) { 
		if (auto mat = GetComponent<MaterialComponent>()) mat->SetBlend(blend); 
	}

	ShaderName GetShader() { 
		if (auto mat = GetComponent<MaterialComponent>()) return mat->GetShader();
		return "ObjectShader"; 
	}
	BlendMode GetBlend() { 
		if (auto mat = GetComponent<MaterialComponent>()) return mat->GetBlend();
		return BlendMode::kBlendModeNone; 
	}

};

