#pragma once
#include <d3dx12.h>
class ViewportScissor
{
private:

	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	
	int kClientWidth_;
	int kClientHeight_;

public:

	ViewportScissor(int kClientWidth, int kClientHeight);

	void CreateViewPort();
	void CreateSxissor();


	D3D12_VIEWPORT* GetViewport() { return &viewport_; }
	D3D12_RECT* GetScissorRect() { return &scissorRect_; }
};

