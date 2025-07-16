#include "Texture.h"
#include "Load.h"




void Texture::Initalize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DescriptorHeap* descriptorHeap, TextureLoader* textureLoader)
{
	device_ = device;
	commandList_ = commandList;
	textureLoader_ = textureLoader;
	descriptorHeap_ = descriptorHeap;
}

void Texture::CreateTexture(const std::string& filePath)
{
	//Textureを読み込んで転送する//
	DirectX::ScratchImage mipImages = LoadTexture(filePath);

	if (textureLoader_->CheckFilePath(filePath)) {
		return;
	}

	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device_, metaData);
	ID3D12Resource* intermediateResource = UploadTextureData(textureResource, mipImages, device_, commandList_);

	//実際にShaderResourceView

	//meteDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV(), textureLoader_->GetLastIndex() + 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV(), textureLoader_->GetLastIndex() + 1);

	textureLoader_->StockTextureData(filePath, textureSrvHandleCPU, textureSrvHandleGPU);

	device_->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
}


