#pragma once
#include <d3d12.h> 

class DepthStencilState
{
private:
	
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

public:
	 
	void CreateDepthStencilState();

	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(){return depthStencilDesc_; }
};

