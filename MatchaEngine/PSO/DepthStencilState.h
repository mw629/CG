#pragma once
#include <d3d12.h> 
#include "RenderState.h"
#include <string>

class DepthStencilState
{
private:
	
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

public:
	 
	void CreateDepthStencilState(const std::string& shader);

	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(){return depthStencilDesc_; }
};

