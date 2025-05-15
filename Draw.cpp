#include "Draw.h"
#include "GraphicsDevice.h"

void Draw::CreateTriangle(ID3D12Device* device)
{
	GraphicsDevice graphicsDevice;

	//VertexResourceを生成する//
	ID3D12Resource* vertexResource = graphicsDevice.CreateBufferResource(device, sizeof(VertexData) * 6);

	//VertexxBuffViewを作成する//

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);


	//Resouceにデータを書き込む//

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//1枚目

	//左下
	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

	//2枚

	//左下
	vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	//上
	vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };



	//Sprote用の頂点リソースを作る//

	ID3D12Resource* vertexResourceSprite = graphicsDevice.CreateBufferResource(device, sizeof(VertexData) * 6);

	//頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//リソースの先頭アドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);


	
}

void Draw::DrawTriangle(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,Transform camraTransform, Transform transform){

	GraphicsDevice graphicsDevice;
	//WVP用のリソースを作る
	ID3D12Resource* wvpResource = graphicsDevice.CreateBufferResource(device, sizeof(Matrix4x4));
	//
	Matrix4x4* wvpData = nullptr;
	//
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//
	*wvpData = IdentityMatrix();

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.translate, transform.scale, transform.rotate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(camraTransform.translate, camraTransform.scale, camraTransform.rotate);
	Matrix4x4 viewMatrix = InverseMatrix4x4(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatrix));
	*wvpData = worldViewProjectionMatrix;//camera?


	commandList->DrawInstanced(6, 1, 0, 0);
}
