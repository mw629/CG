#include "TextureLoader.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI

bool TextureLoader::CheckFilePath(const std::string& filePath)
{
	return textureMap_.find(filePath) != textureMap_.end();
}

bool TextureLoader::StockTextureData(const std::string& filePath, 
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU,
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource,	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource)
{
	Texture tex;
	tex.index = static_cast<int>(textureKeys_.size());//0を基準
	tex.filePath = filePath;
	tex.textureSrvHandleCPU = textureSrvHandleCPU;
	tex.textureSrvHandleGPU = textureSrvHandleGPU;
	tex.resource = textureResource;
	tex.intermediateResource = intermediateResource;

	textureMap_[filePath] = tex;
	textureKeys_.push_back(filePath);

	return true;
}

void TextureLoader::Draw() {
#ifdef _USE_IMGUI
if (ImGui::Begin("Texture List")) {
		for (const auto& key : textureKeys_) {
			const auto& tex = textureMap_[key];
			ImGui::Text("%d | %s", tex.index, tex.filePath.c_str());
		}
	}
	ImGui::End();
#endif // _USE_IMGUI

}

int TextureLoader::GetTextureIndex(const std::string& filePath)
{
	auto it = textureMap_.find(filePath);
	if (it != textureMap_.end()) {
		return it->second.index;
	}
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureLoader::GetTexture(const std::string& filePath)
{
	auto it = textureMap_.find(filePath);
	if (it != textureMap_.end()) {
		return it->second.textureSrvHandleGPU;
	}
	return textureMap_[textureKeys_[0]].textureSrvHandleGPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureLoader::GetTexture(int index)
{
	if (index >= 0 && index < textureKeys_.size()) {
		return textureMap_[textureKeys_[index]].textureSrvHandleGPU;
	}
	return textureMap_[textureKeys_[0]].textureSrvHandleGPU;
}



