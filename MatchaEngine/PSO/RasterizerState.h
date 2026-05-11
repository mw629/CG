#pragma once  
#include <d3d12.h> 
#include "RenderState.h"
#include <string>

class RasterizerState  
{  
private:  
    D3D12_RASTERIZER_DESC rasterizerDesc_{};  
public:  
    void CreateRasterizerState(const std::string& shader);  

    
    D3D12_RASTERIZER_DESC GetRasterizerDesc() const { return rasterizerDesc_; };
};
