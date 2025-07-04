#pragma once
#include <wrl.h>
#include "../Common/VariableTypes.h"
#include "Matrial.h"
#include <d3dx12.h>


class Sphere
{
private:
	Transform transform_;
	Matrial* matrial_;
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

	void Initialize(Matrial* matrial, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

	void CreateVertexData(ID3D12Device* device);

	void CreateIndexResource(ID3D12Device* device);

	void CreateWVP(ID3D12Device* device);

	void CreateSprite(ID3D12Device* device);

	void SetWvp(Matrix4x4 viewMatrix);

	void SetTrandform(Transform transform);


	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource() { return wvpResource_.Get(); }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }


	Matrial* GetMatrial() { return matrial_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }

	uint32_t GetSubdivision() { return kSubdivision_; }

};

