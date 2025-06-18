#include "RenderTargetView.h"
#include <array>

void RenderTargetView::CreateRenderTargetView(ID3D12Device *device, std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> &swapChainResources, ID3D12DescriptorHeap* rtvDescriptorHeap)
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
