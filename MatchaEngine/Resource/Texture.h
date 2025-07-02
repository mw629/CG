#pragma once  
#include <d3dx12.h>  
#include <DirectXTex.h>
#include <wrl.h>

#include "../Common/GraphicsDevice.h"
#include "TextureLoader.h"
#include "../../DescriptorHeap.h"

class Texture
{
private:

	ID3D12Device* device_;
	TextureLoader* textureLoader_;
	DescriptorHeap* descriptorHeap_;
	ID3D12GraphicsCommandList* commandList_;

public:
	void Initalize(ID3D12Device* graphicsDevice, ID3D12GraphicsCommandList* commandList, DescriptorHeap *descriptorHeap, TextureLoader* textureLoader);

	void CreateTexture(const std::string& filePath);
};
