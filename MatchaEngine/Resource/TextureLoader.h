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
	};

	std::vector<Texture> texture_;

public:

	bool CheckFilePath(const std::string& filePath);

	bool StockTextureData(
		const std::string& filePath,
		D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

	void Draw();

	int GetLastIndex() { return texture_.size(); }

	std::vector<Texture> GeTexture() { return texture_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTexture(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetTexture(int index);

};

