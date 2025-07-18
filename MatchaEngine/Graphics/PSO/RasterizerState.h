#pragma once  
#include <d3d12.h> 

class RasterizerState  
{  
private:  
    D3D12_RASTERIZER_DESC rasterizerDesc_{};  
public:  
    void CreateRasterizerState();  

    
    D3D12_RASTERIZER_DESC GetRasterizerDesc() const { return rasterizerDesc_; };
};
