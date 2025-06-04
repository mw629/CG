#pragma once
#include "externals/DirectXTex/d3dx12.h"
class BlendState
{
private:
	D3D12_BLEND_DESC blendDesc{};
public:
	void CreateBlendDesc();
};

