#pragma once
#include <d3dx12.h>
#include "SwapChain.h"
class ResourceBarrierHelper
{
private:
    UINT backBufferIndex_;
    D3D12_RESOURCE_BARRIER barrier_{};    

public:
    void Transition(ID3D12GraphicsCommandList* commandList,SwapChain* swapChain);

    void TransitionToPresent(ID3D12GraphicsCommandList* commandList);

    UINT GetBackBufferIndex() { return  backBufferIndex_; }
    D3D12_RESOURCE_BARRIER GetBarrier() { return barrier_; }
};

