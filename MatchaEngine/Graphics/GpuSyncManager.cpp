#include "GpuSyncManager.h"
#include <cassert>



void GpuSyncManager::Initialize(ID3D12Device* device)
{
	//初期値0でFenceを作る
	fence = nullptr;
	fenceValue = 0;
	HRESULT hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを作成する
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);

}

void GpuSyncManager::Signal(ID3D12CommandQueue* queue)
{
	fenceValue++;
	HRESULT hr = queue->Signal(fence.Get(), fenceValue);
	assert(SUCCEEDED(hr));
}

void GpuSyncManager::WaitForGpu()
{
	if (fence->GetCompletedValue() < fenceValue) {
		HRESULT hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
		assert(SUCCEEDED(hr));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}
