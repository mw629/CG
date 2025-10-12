#pragma once
#include <d3dx12.h>
#include <dxgi1_6.h>
#include "Core/VariableTypes.h"


class RenderTargetView
{
private:

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];


	float clearColor[4];

public:
	RenderTargetView();

	void CreateRenderTargetView(ID3D12Device* device, ID3D12Resource* swapChainResources0, ID3D12Resource* swapChainResources1, ID3D12DescriptorHeap* rtvDescriptorHeap);
	
	void SetAndClear(ID3D12GraphicsCommandList* commandList, UINT backBufferIndex);

	void RecreateRenderTargetViews(ID3D12Device* device, ID3D12Resource* newSwapChainResources0, ID3D12Resource* newSwapChainResources1, ID3D12DescriptorHeap* rtvDescriptorHeap);

	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc_; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvStartHandle_() { return rtvStartHandle_; }
	D3D12_CPU_DESCRIPTOR_HANDLE* GetRtvHandles(int index) { return &rtvHandles_[index]; };

	void ResetRenderTarget();

	};

