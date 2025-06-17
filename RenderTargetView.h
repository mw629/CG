#pragma once
#include <d3dx12.h>
#include <dxgi1_6.h>


class RenderTargetView
{
private:

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2] = { nullptr };

public:

	void CreateRenderTargetView(ID3D12Device* device, ID3D12Resource* swapChainResources[2], ID3D12DescriptorHeap* rtvDescriptorHeap);
};

