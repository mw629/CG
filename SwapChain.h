#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3dx12.h>

class SwapChain
{
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	HRESULT hr_;
public:


	void CreateSwapChain(IDXGIFactory7* DxgiFactory, ID3D12CommandQueue* CommandQueue, HWND Hwnd, int kClientWidth, int kClientHeight);

    IDXGISwapChain4* GetSwapChain() { return swapChain_.Get(); }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }

};

