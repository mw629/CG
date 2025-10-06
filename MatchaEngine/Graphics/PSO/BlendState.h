#pragma once
#include <d3d12.h> 
#include "RenderState.h"

class BlendState
{
private:

	D3D12_BLEND_DESC blendDesc_{};

public:

	D3D12_BLEND_DESC CreateBlendDesc(BlendMode mode);

	D3D12_BLEND_DESC GetBlendDesc() { return blendDesc_; }
};

