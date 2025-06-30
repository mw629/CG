#include "Texture.h"
#include "Load.h"

void Texture::Initalize(GraphicsDevice* graphicsDevice)
{
	graphicsDevice_=graphicsDevice;
}

void Texture::CreateTexture()
{ 
	////Textureを読み込んで転送する//
	//mipImages = LoadTexture("resources/uvChecker.png");
	//metaData = mipImages.GetMetadata();
	//Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(graphicsDevice_->GetDevice(), metaData);
	//Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mipImages, graphicsDevice_->GetDevice(), command.get()->GetCommandList());

	////実際にShaderResourceView

	////meteDataを基にSRVの設定
	//srvDesc.Format = metaData.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

	////SRVを作成するDescriptorHeapの場所を決める
	//textureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
	//textureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);

	//graphicsDevice_->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
}


