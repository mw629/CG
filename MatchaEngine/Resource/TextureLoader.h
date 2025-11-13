#pragma once
#include <d3dx12.h>
class TextureLoader
{
private:

	struct Texture
	{
		int index;
		std::string filePath;
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource; 
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
	};

	std::vector<Texture> texture_;

public:

	bool CheckFilePath(const std::string& filePath);

	bool StockTextureData(const std::string& filePath,
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU,
		Microsoft::WRL::ComPtr<ID3D12Resource> textureResource, 
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource);



	void Draw();

	int GetLastIndex() { return static_cast<int>(texture_.size()); }

	int GetTextureIndex(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetTexture(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetTexture(int index);

};

