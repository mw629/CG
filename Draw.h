#pragma once  
#include "VariableTypes.h"  
#include "Calculation.h"  
#include <d3d12.h>  
#include <cstdint>  
#include "Camera.h"

class Draw {
private:

	ID3D12Resource* vertexResource;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	VertexData* vertexData = nullptr;
	ID3D12Resource* wvpResource;
	Matrix4x4* wvpData;

	Camera camera;


public:

	Draw();
	~Draw();

	void Initialize();

	void DrawTriangle(Transform transform,
		Transform cameraTransform,
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		ID3D12Resource* materialResource,
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);
};
