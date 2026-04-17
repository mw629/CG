#pragma once  
#include <d3d12.h> 
#include "RenderState.h"

class RasterizerState  
{  
private:  
    D3D12_RASTERIZER_DESC rasterizerDesc_{};  
public:  
    void CreateRasterizerState(ShaderName shader);  

    
    D3D12_RASTERIZER_DESC GetRasterizerDesc() const { return rasterizerDesc_; };
};
