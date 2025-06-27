#include "Draw.h"
#include "Common/GraphicsDevice.h"
#include <cassert>


Draw::Draw()
{
}

Draw::~Draw()
{
	Initialize();
}

void Draw::Initialize()
{
	
}

void Draw::DrawTriangle(Transform transform,
	Transform cameraTransform,
	ID3D12Device* device, 
	ID3D12GraphicsCommandList* commandList,
	ID3D12Resource* materialResource,
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU)
{

	// VertexResourceを生成する//
	vertexResource = GraphicsDevice::CreateBufferResource(device, sizeof(VertexData) * 3);


	//VertexxBuffViewを作成する//

	//頂点バッファビューを作成する
	vertexBufferView;
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 3;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//Resouceにデータを書き込む//

	//左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };


	//WVP用のリソースを作る
	wvpResource = GraphicsDevice::CreateBufferResource(device, sizeof(Matrix4x4));
	//
	wvpData = nullptr;
	//
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//
	//*wvpData = camera.MakeWorldViewProjectionMatrix(transform, cameraTransform);

	

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	//描画！（DrawCall/ドローコール）。3頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(3, 1, 0, 0);
}

void Draw::DrawObj()
{
	//command.get()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewObj);//VBVを設定
	//command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteMatrial.get()->GetMaterialResource()->GetGPUVirtualAddress());
	//command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceObj->GetGPUVirtualAddress());
	//command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	//command.get()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
}
