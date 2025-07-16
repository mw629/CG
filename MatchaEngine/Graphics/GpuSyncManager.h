#pragma once
#include <d3dx12.h>
class GpuSyncManager
{
private:
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;
	
public:
	void Initialize(ID3D12Device* device);
	void Signal(ID3D12CommandQueue* queue);
	void WaitForGpu();
};

