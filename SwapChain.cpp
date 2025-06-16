#include "SwapChain.h"
#include <cassert>



SwapChain::SwapChain(IDXGIFactory7* DxgiFactory, ID3D12CommandQueue* CommandQueue, HWND Hwnd, int kClientWidth, int kClientHeight)
{
	CreateSwapChain(DxgiFactory, CommandQueue,Hwnd,kClientWidth,kClientHeight);
}

void SwapChain::CreateSwapChain(IDXGIFactory7 *DxgiFactory, ID3D12CommandQueue* CommandQueue, HWND Hwnd,int kClientWidth,int kClientHeight)
{
	swapChainDesc_.Width = kClientWidth;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = kClientHeight;//画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc_.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画ターゲットとして利用する
	swapChainDesc_.BufferCount = 2;//ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら、中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する

	hr_ = DxgiFactory->CreateSwapChainForHwnd(CommandQueue,Hwnd, &swapChainDesc_,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));

	assert(SUCCEEDED(hr_));
}
