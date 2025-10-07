#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "../Core/VariableTypes.h"
#include "MaterialFactory.h"

class Particle
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

	static void SetDevice(ID3D12Device* device);
	static void SetScreenSize(Vector2 screenSize);

	void Initialize(ModelData modelData);

	void CreateVertexData();
	void CreateIndexResource();
	void CreateWVP();

	void SettingWvp(Matrix4x4 viewMatrix);
	void SetTransform(Transform transform);

	void CreateModel();


};

