#include "Particle.h"
#include <Math/Calculation.h>
#include "Graphics/GraphicsDevice.h"

namespace {
	ID3D12Device* device_;
	float kClientWidth;
	float kClientHeight;
}


void Particle::SetDevice(ID3D12Device* device)
{
	device_ = device;
}

void Particle::SetScreenSize(Vector2 screenSize)
{
	kClientWidth = screenSize.x;
	kClientHeight = screenSize.y;
}


void Particle::CreateVertexData()
{
	//Sprote用の頂点リソースを作る//

	vertexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(VertexData) * 6);

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


void Particle::CreateIndexResource()
{
	indexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(uint32_t) * 6); ;


	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

}

void Particle::CreateWVP()
{

	wvpResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(TransformationMatrix));
	//データを書き込む
	wvpData_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->World = IdentityMatrix();
	wvpData_->WVP = IdentityMatrix();
}


void Particle::CreateModel()
{
	CreateVertexData();
	CreateIndexResource();
	CreateWVP();
}

void Particle::SettingWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldMatrixObj = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrixObj,worldMatrixObj };
}

void Particle::SetTransform(Transform transform)
{
	transform_.translate = transform.translate;
	transform_.scale = transform.scale;
	transform_.rotate = transform.rotate;
}