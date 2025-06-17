#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
class DescriptorHeap
{
private:
	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

public:
	DescriptorHeap(ID3D12Device* device);

	void CreateHeap(ID3D12Device* device);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeep(
		ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesxriptors, bool shaderVisible);

	uint32_t GetDescriptorSizeSRV() { return descriptorSizeSRV_; }
	uint32_t GetDescriptorSizeRTV() { return  descriptorSizeRTV_; }
	uint32_t GetDescriptorSizeDSV() { return descriptorSizeDSV_; }

	ID3D12DescriptorHeap* GetRtvDescriptorHeap() { return rtvDescriptorHeap_.Get(); }
	ID3D12DescriptorHeap* GetSrvDescriptorHeap() { return srvDescriptorHeap_.Get(); }
	ID3D12DescriptorHeap* GetDsvDescriptorHeap() { return dsvDescriptorHeap_.Get(); }
};

