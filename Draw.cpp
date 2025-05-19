#include "Draw.h"
#include "GraphicsDevice.h"
#include <cassert>


Draw::Draw()
{
}

Draw::~Draw()
{
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
	GraphicsDevice graphicsDevice;
	// VertexResourceを生成する//
	vertexResource = graphicsDevice.CreateBufferResource(device, sizeof(VertexData) * vertexIndex);


	//VertexxBuffViewを作成する//

	//頂点バッファビューを作成する

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexIndex;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//Resouceにデータを書き込む//

	//左下
	vertexData[0].position = { -0.1f,-0.1f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData[1].position = { 0.0f,0.1f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[2].position = { 0.1f,-0.1f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };
	
	//2枚

	//左下
	vertexData[3].position = { 0.0f,-0.1f,-0.1f,1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	//上
	vertexData[4].position = { 0.0f,0.1f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.1f,0.0f };
	//右下
	vertexData[5].position = { -0.1f,-0.1f,0.0f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };

	//3枚
	
	//左下
	vertexData[6].position = { 0.1f,-0.1f,0.0f,1.0f };
	vertexData[6].texcoord = { 0.0f,1.0f };
	//上
	vertexData[7].position = { 0.0f,0.1f,0.0f,1.0f };
	vertexData[7].texcoord = { 0.1f,0.0f };
	//右下
	vertexData[8].position = { 0.0f,-0.1f,-0.1f,1.0f };
	vertexData[8].texcoord = { 1.0f,1.0f };

	//WVP用のリソースを作る
	wvpResource = graphicsDevice.CreateBufferResource(device, sizeof(Matrix4x4));
	//
	wvpData = nullptr;
	//
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//
	*wvpData = camera.MakeWorldViewProjectionMatrix(transform, cameraTransform);

	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定

	//マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	//描画！（DrawCall/ドローコール）。3頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(vertexIndex, 1, 0, 0);
}
