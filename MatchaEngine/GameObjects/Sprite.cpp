#include "Sprite.h"
#include <Common/GraphicsDevice.h>
#include <Common/VariableTypes.h>
#include <Math/Calculation.h>

Sprite::Sprite()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
}

void Sprite::CreateVertexData(ID3D12Device* device)
{
	//Sprote用の頂点リソースを作る//

	vertexResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(VertexData) * 6);

	//頂点バッファービューを作成する
	
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	//頂点データを設定する//
	
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//１枚目の三角形
	vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexData_[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexData_[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexData_[3].texcoord = { 1.0f,0.0f };

	
}

void Sprite::CreateWVP(ID3D12Device* device)
{
	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	wvpResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(TransformationMatrix));
	//データを書き込む
	
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->World = IdentityMatrix();
	wvpData_->WVP = IdentityMatrix();
}

void Sprite::SetWvp()
{
	Matrix4x4 viewMatrix = IdentityMatrix();
	Matrix4x4 projectionMatri = MakeOrthographicMatrix(0.0f, 1280.0f, 0.0f, 720.0f, 0.0f, 1.0f);
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix4x4(worldMatrixSprite, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrixSprite,worldMatrixSprite };

}

void Sprite::SetTrandform(Transform transform)
{
	transform_.translate = transform.translate;
	transform_.scale = transform.scale;
	transform_.rotate = transform.rotate;
}


