#pragma once
#include <wrl.h>
#include <d3dx12.h>

class DepthStencil
{
private:

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_{};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_{};

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_{};

	D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_DEPTH_WRITE;

public:
	~DepthStencil();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeep(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesxriptors, bool shaderVisible);


	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	void CreateDepthStencil(ID3D12Device* device, int32_t width, int32_t height, ID3D12DescriptorHeap* srvDescriptorHeap = nullptr, uint32_t srvDescriptorSize = 0);

	void SetDSV(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE* RtvHandles);

	void TransitionToShaderResource(ID3D12GraphicsCommandList* commandList);
	void TransitionToDepthWrite(ID3D12GraphicsCommandList* commandList);


	ID3D12Resource* GetDepthStencilResource() { return depthStencilResource_.Get(); }
	ID3D12DescriptorHeap* GetDsvDescriptorHeap() { return dsvDescriptorHeap_.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvHandleCPU() const { return srvHandleCPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU() const { return srvHandleGPU_; }
};
