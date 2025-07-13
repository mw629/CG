#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "Core/VariableTypes.h"
#include "Resource/Texture.h"
#include "MaterialFactory.h"


class Model
{
private:
	ModelData modelData_;
	Transform transform_;
	MaterialFactory* material_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpDataResource_;
	TransformationMatrix* wvpData_ = nullptr;


	bool isAlive = true;
	int kClientWidth = 1280;
	int kClientHeight = 720;

public:
	~Model();


	void Initialize(ModelData modelData, MaterialFactory* matrial, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

	void CreateVertexData(ID3D12Device* device);
	void CreateWVP(ID3D12Device* device);

	
	void SetWvp(Matrix4x4 viewMatrix);
	void SetTransform(Transform transform);

	void CreateModel(ID3D12Device* device);


	
	void SetPos(Vector3 velocity) { transform_.translate = velocity; }
	void SetAlive(bool flag) { isAlive = flag; }

	//getter
	bool GetAlive() { return isAlive; }
	Transform GetTransform() { return transform_; }
	
	ModelData GetModelData() { return modelData_; }
	MaterialFactory* GetMatrial() { return material_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU_; }


	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetWvpDataResource() { return wvpDataResource_.Get(); }
	TransformationMatrix* GetWvpData() { return wvpData_; }



	
	
};

