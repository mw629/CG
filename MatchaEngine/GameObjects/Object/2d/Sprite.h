#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "Core/VariableTypes.h"
#include "MaterialFactory.h"
#include "PipelineState.h"
#include "../Object/GameObject.h"
#include "../Component/MaterialComponent.h"

struct SpriteData{
	Transform transform;
	Vector2 size;
	Vector2 textureArea[2];
};

class Sprite : public GameObject
{
private:

	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_;


	Vector2 size_ = { 200,200 };
	Vector2 textureArea_[2] = { {0.0f,0.0f},{1.0f,1.0f} };

	Vector2 leftTop_;
	Vector2 rigthBottom_;
	Vector2 textureLeftTop_;
	Vector2 textureRigthBottom_;


	
public:
	Sprite();
	~Sprite();

	static void SetScreenSize(Vector2 screenSize);

	void Initialize(SpriteData spriteData, int textureHandle);

	void CreateVertexData();

	void CreateIndexResource();

	void CreateWVP();

	void CreateSprite();

	void SettingWvp();

	void SetTransform(Transform transform);

	void SetSize(Vector2 size);

	void SetTextureArea(Vector2 textureArea[2]);

	void Update(SpriteData spriteData);

	void SetMaterialLighting(bool isActiv) { 
		auto matComp = GetComponent<MaterialComponent>();
		if(matComp) matComp->GetMaterialFactory()->SetMaterialLighting(isActiv);
	}

	void SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) { textureSrvHandleGPU_ = textureSrvHandleGPU; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource() { return wvpResource_.Get(); }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }

	Transform& GetTransform() { return transform_; }
	
	MaterialFactory* GetMartial() { 
		auto matComp = GetComponent<MaterialComponent>();
		return matComp ? matComp->GetMaterialFactory() : nullptr;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }

private:
	ShaderName shader_ = "ObjectShader";
	BlendMode blend_ = BlendMode::kBlendModeNormal;
public:
	void SetBlend(BlendMode blend) { blend_ = blend; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }

};



