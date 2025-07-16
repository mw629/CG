#pragma once
#include <d3d12.h> 

class BlendState
{
private:
	D3D12_BLEND_DESC blendDesc_{};
public:
	void CreateBlendDesc();

	D3D12_BLEND_DESC GetBlendDesc() { return blendDesc_; }
};

