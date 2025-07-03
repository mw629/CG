#pragma once
#include <wrl.h>
#include <d3dx12.h>

class DepthStencil
{
private:

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

public:

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeep(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesxriptors, bool shaderVisible);


	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

	void CreateDepthStencil(ID3D12Device* device, int32_t width, int32_t height);

	void SetDSV(ID3D12GraphicsCommandList* commandList, D3D12_CPU_DESCRIPTOR_HANDLE* RtvHandles);


	ID3D12Resource* GetDepthStencilResource() { return depthStencilResource_.Get(); }
	ID3D12DescriptorHeap* GetDsvDescriptorHeap() { return dsvDescriptorHeap_.Get(); }

};
