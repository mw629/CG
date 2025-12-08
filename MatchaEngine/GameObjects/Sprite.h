#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Core/VariableTypes.h"
#include "MaterialFactory.h"
#include "RenderState.h"

struct SpriteData{
	Transform transform;
	Vector2 size;
	Vector2 textureArea[2];
};

class Sprite
{
private:

	Transform transform_;
	MaterialFactory* material_;
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

	static void SetDevice(ID3D12Device* device);
	static void SetScreenSize(Vector2 screenSize);

	void ImGui();

	void Initialize(SpriteData spriteData, int textureHandle);

	void CreateVertexData();

	void CreateIndexResource();

	void CreateWVP();

	void CreateSprite();

	void SettingWvp();

	void SetTransform(Transform transform);

	void SetSize(Vector2 size);

	SpriteData SetData(Vector2* const size);

	void SetTextureArea(Vector2 textureArea[2]);

	void Update(SpriteData spriteData);

	void SetMaterialLighting(bool isActiv) { material_->SetMaterialLighting(isActiv); }

	void SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) { textureSrvHandleGPU_ = textureSrvHandleGPU; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource() { return wvpResource_.Get(); }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }

	Transform& GetTransform() { return transform_; }
	
	MaterialFactory* GetMatrial() { return material_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }

private:
	ShaderName shader_ = ShaderName::SpriteShader;
	BlendMode blend_ = BlendMode::kBlendModeNormal;
public:
	BlendMode SetBlend(BlendMode blend) { blend_ = blend; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }

};



