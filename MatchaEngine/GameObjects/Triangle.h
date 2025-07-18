#pragma once
#include <Core/VariableTypes.h>
#include "MaterialFactory.h"
class Triangle
{
private:

	Transform transform_{};
	Vector4 vertex_[3];
	MaterialFactory* material_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_{};
	TransformationMatrix* wvpData_{};


public:
	~Triangle();

	void Initialize(MaterialFactory* matrial, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);
	
	void CreateVertexData(ID3D12Device* device);

	void CreateWVP(ID3D12Device* device);

	void CreateTriangle(ID3D12Device* device);

	void SetWvp(Matrix4x4 viewMatrix);

	void SetShape();

	void SetTrandform(Transform transform);

	void SetVertex(Vector4 vertex[3]);


	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource()const { return wvpResource_.Get(); }

	MaterialFactory* GetMatrial()const { return material_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }


};

