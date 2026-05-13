#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <cstdint>

class RenderTexture
{
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandleCPU_{};
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_{};
	float clearColor_[4];

public:
	RenderTexture();

	void Initialize(ID3D12Device* device, int width, int height, ID3D12DescriptorHeap* srvDescriptorHeap, uint32_t srvDescriptorSize);
	void Clear(ID3D12GraphicsCommandList* commandList);
	void TransitionToRenderTarget(ID3D12GraphicsCommandList* commandList);
	void TransitionToShaderResource(ID3D12GraphicsCommandList* commandList);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle() const { return rtvHandleCPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU() const { return srvHandleGPU_; }
	ID3D12Resource* GetResource() const { return resource_.Get(); }
};

