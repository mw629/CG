#pragma once  
#include <d3dx12.h>  
#include <DirectXTex.h>
#include <wrl.h>

#include "../Graphics/GraphicsDevice.h"
#include "TextureLoader.h"
#include "../Graphics/DescriptorHeap.h"

class Texture
{
public:

	static void Initalize(ID3D12Device* graphicsDevice, ID3D12GraphicsCommandList* commandList, DescriptorHeap* descriptorHeap, TextureLoader* textureLoader);

	int CreateTexture(const std::string& filePath);

	void TextureList();

	D3D12_GPU_DESCRIPTOR_HANDLE TextureData(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE TextureData(int index);
};
