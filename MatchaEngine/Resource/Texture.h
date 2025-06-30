#pragma once  
#include <d3dx12.h>  
#include <DirectXTex.h>
#include <wrl.h>

#include "../Common/GraphicsDevice.h"

class Texture
{
private:

	GraphicsDevice* graphicsDevice_;

	DirectX::ScratchImage mipImages;
	DirectX::TexMetadata& metaData;

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

public:
	void Initalize(GraphicsDevice* graphicsDevice);

	void CreateTexture();
};
