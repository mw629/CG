#include "Texture.h"
#include "MatchaEngine/Resource/Load.h"

//void Texture::LoadTextureData()
//{
//	//Textureを読み込んで転送する//
//	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
//	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
//	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(graphics.get()->GetDevice(), metaData);
//	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mipImages, graphics.get()->GetDevice(), command.get()->GetCommandList());
//	//実際にShaderResourceView
//	//meteDataを基にSRVの設定
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	srvDesc.Format = metaData.format;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
//	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
//	//SRVを作成するDescriptorHeapの場所を決める
//	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
//	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
//	graphics.get()->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
//}
