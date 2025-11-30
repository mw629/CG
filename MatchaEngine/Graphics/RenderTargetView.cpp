#include "RenderTargetView.h"
#include <array>

RenderTargetView::RenderTargetView()
{
	clearColor[0] = 0.1f;
	clearColor[1] = 0.25f;
	clearColor[2] = 0.5f;
	clearColor[3] = 1.0f;//青っぽい色。RGBAの順

	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 1.0f;//青っぽい色。RGBAの順
}

void RenderTargetView::CreateRenderTargetView(ID3D12Device* device, std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> swapChainResources, ID3D12DescriptorHeap* rtvDescriptorHeap)
{
	//RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する(ここが変わるかも05-01)
	rtvStartHandle_ = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//まず一つ目作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle_;
	device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc_, rtvHandles_[0]);
	//2つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc_, rtvHandles_[1]);

}

void RenderTargetView::SetAndClear(ID3D12GraphicsCommandList* commandList, UINT backBufferIndex)
{
	//描画先のRTVを設定する
	commandList->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//指定した色で画面全体をクリアする
	commandList->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);
}

void RenderTargetView::RecreateRenderTargetViews(ID3D12Device* device,
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>newSwapChainResources,
	ID3D12DescriptorHeap* rtvDescriptorHeap)
{
	CreateRenderTargetView(device, newSwapChainResources, rtvDescriptorHeap);
}