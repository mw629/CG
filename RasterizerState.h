#pragma once
#include "externals/DirectXTex/d3dx12.h"
class RasterizerState
{
private:
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
public:
	void CreateRasterizerState();

	D3D12_RASTERIZER_DESC GetRasterizerDesc() { return rasterizerDesc_; };
};

