#pragma once
#include <wrl.h>
#include "../Core/VariableTypes.h"
#include "MaterialFactory.h"
#include <d3dx12.h>


class Sphere
{
private:
	Transform transform_;
	MaterialFactory* material_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	uint32_t kSubdivision_ = 16;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_;
public:
	Sphere();
	~Sphere();

	static void SetDevice(ID3D12Device* device);

	void Initialize(MaterialFactory* material, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

	void CreateVertexData();

	void CreateIndexResource();

	void CreateWVP();

	void CreateSprite();

	void SetWvp(Matrix4x4 viewMatrix);

	void SetTrandform(Transform transform);


	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource() { return wvpResource_.Get(); }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }


	MaterialFactory* GetMatrial() { return material_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }

	uint32_t GetSubdivision() { return kSubdivision_; }

};

