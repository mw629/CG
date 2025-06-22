#include "Model.h"
#include "Common/GraphicsDevice.h"
#include "Math/Calculation.h"
#include "../Resource/Load.h"
#include <fstream>
#include <sstream>



Model::Model(ModelData modelData)
{
	transform_={};
	modelData_ = modelData;
}


void Model::CreateVertexData(ID3D12Device* device)
{
	//頂点リソースを作る
	vertexResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(VertexData) * modelData_.vertices.size());
	//頂点バッファービューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * modelData_.vertices.size();
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Model::CreateWVP(ID3D12Device* device)
{
	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	wvpDataResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む

	//書き込むためのアドレスを取得
	wvpDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->WVP = Identity();
	wvpData_->World = Identity();
}

//void Model::SetTexture(ID3D12Device* device)
//{
//	//二枚目のTextureを読み込む
//	DirectX::ScratchImage mapImages = LoadTexture(modelData_.material.textureDilePath);
//	const DirectX::TexMetadata& metaData = mapImages.GetMetadata();
//	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(device, metaData);
//	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mapImages, device, command.get()->GetCommandList());
//
//
//	//metaDataを基にSRVの設定
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	srvDesc.Format = metaData.format;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
//	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
//
//	//SRVを作成するDescriptorHeapの場所を決める
//	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
//	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
//	
//	device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
//
//}

void Model::CreateModel(ID3D12Device* device)
{
	CreateVertexData(device);
	CreateWVP(device);
}

void Model::SetWvp(Transform camera)
{
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camera.translate, camera.scale, camera.rotate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldMatrixObj = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrixObj = Multiply(worldMatrixObj, Multiply(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrixObj,worldMatrixObj };
}

void Model::SetTransform(Transform transform)
{
	transform_.translate = transform.translate;
	transform_.scale = transform.scale;
	transform_.rotate = transform.rotate;
}
