#pragma once
#include <d3d12.h> 

enum BlendMode {
	kBlendModeNone,
	kBlendModeNormal,
	kBlendModeAdd,
	kBlendModeSubtract,
	kBlendModeMultiply,
	kBlendModeScreen
};

class BlendState
{
private:

	D3D12_BLEND_DESC blendDesc_{};

public:

	D3D12_BLEND_DESC CreateBlendDesc(BlendMode mode);

	D3D12_BLEND_DESC GetBlendDesc() { return blendDesc_; }
};

