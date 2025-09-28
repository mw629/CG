#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Core/VariableTypes.h"
#include "../GameObjects/MaterialFactory.h"

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

	
public:
	Sprite();
	~Sprite();

	static void SetDevice(ID3D12Device* device);

	void Initialize(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

	void CreateVertexData();

	void CreateIndexResource();

	void CreateWVP();

	void CreateSprite();

	void SettingWvp();

	void SetSize(Vector2 leftTop, Vector2 rigthBottom);

	void SetMaterialLighting(bool isActiv) { material_->SetMaterialLighting(isActiv); }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource() { return wvpResource_.Get(); }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }

	
	MaterialFactory* GetMatrial() { return material_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }

};


