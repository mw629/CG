#pragma once
#include "externals/DirectXTex/d3dx12.h"
class RasterizerState
{
private:
	D3D12_RASTERIZER_DESC rasterizerDesc{};
public:
	void CreateRasterizerState();
};

