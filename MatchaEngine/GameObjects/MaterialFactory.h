#pragma once  
#include <wrl.h>  
#include <d3d12.h>  
#include "../Core/VariableTypes.h"

class MaterialFactory  
{  
private:  
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_{};
    Material* materialData_{};

public:  
    ~MaterialFactory();

    static void SetDevice(ID3D12Device* device);

    void CreateMatrial(bool Lighting);  

    ID3D12Resource* GetMaterialResource() { return materialResource_.Get(); }  
    Material* GetMaterialData() { return materialData_; }  
};
