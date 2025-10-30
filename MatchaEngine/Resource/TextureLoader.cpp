#include "TextureLoader.h"
#include "../externals/imgui/imgui.h"

bool TextureLoader::CheckFilePath(const std::string& filePath)
{
	for (int i = 0; i < texture_.size(); i++) {
		if (texture_[i].filePath == filePath) {
			return true;
		}
	}
	return false;
}

bool TextureLoader::StockTextureData(const std::string& filePath, 
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU,
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource,	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource)
{
    Texture tex;
	tex.index = static_cast<int>(texture_.size());//0を基準
    tex.filePath = filePath;
    tex.textureSrvHandleCPU = textureSrvHandleCPU;
    tex.textureSrvHandleGPU = textureSrvHandleGPU;
	tex.resource = textureResource;
	tex.intermediateResource = intermediateResource;

    texture_.push_back(tex); // ベクターに追加

    return true;
}

void TextureLoader::Draw() {
	if (ImGui::Begin("Texture List")) {
		for (const auto& tex : texture_) {
			ImGui::Text("%d | %s", tex.index, tex.filePath.c_str());
		}
	}
	ImGui::End();
}


int TextureLoader::GetTextureIndex(const std::string& filePath)
{
	int number = 0;
	for (int i = 0; i < texture_.size(); i++) {
		if (texture_[i].filePath == filePath) {
			return  i;
		}
	}
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureLoader::GetTexture(const std::string& filePath)
{
	int number = 0;
	for (int i = 0; i < texture_.size(); i++) {
		if (texture_[i].filePath == filePath) {
			number = i;
		}
	}
	return texture_[number].textureSrvHandleGPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureLoader::GetTexture(int index)
{
	int number = 0;
	for (int i = 0; i < texture_.size(); i++) {
		if (texture_[i].index == index) {
			number = i;
		}
	}
	return texture_[number].textureSrvHandleGPU;
}



