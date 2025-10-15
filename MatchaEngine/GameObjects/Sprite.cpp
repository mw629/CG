#include "Sprite.h"
#include "Graphics/GraphicsDevice.h"
#include "Core/VariableTypes.h"
#include "Math/Calculation.h"

namespace {
	ID3D12Device* device_;
}

void Sprite::SetDevice(ID3D12Device* device) {
	device_ = device;
}

Sprite::Sprite()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
}

Sprite::~Sprite()
{
	// MapしたポインタをUnmapする
	if (vertexData_) {
		vertexResource_->Unmap(0, nullptr);
	}
	if (indexData_) {
		indexResource_->Unmap(0, nullptr);
	}
	if (wvpData_) {
		wvpResource_->Unmap(0, nullptr);
	}
	// ComPtrは自動的に解放される
	vertexResource_.Reset();
	indexResource_.Reset();
	wvpResource_.Reset();

	delete material_;
}

void Sprite::Initialize(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU)
{
	textureSrvHandleGPU_ = textureSrvHandleGPU;

	material_ = new MaterialFactory();
	material_->CreateMatrial(device_, false);
}

void Sprite::CreateVertexData()
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

void Sprite::CreateIndexResource()
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

void Sprite::CreateWVP()
{
	
	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	wvpResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(TransformationMatrix));
	//データを書き込む
	wvpData_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->World = IdentityMatrix();
	wvpData_->WVP = IdentityMatrix();
}

void Sprite::CreateSprite()
{
	CreateVertexData();
	CreateIndexResource();
	CreateWVP();
}

void Sprite::SettingWvp()
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(IdentityMatrix(), MakeOrthographicMatrix(0, float(1280), 0, float(720), 0.0f, 100.0f)));
	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };
}

void Sprite::SetSize(Vector2 leftTop, Vector2 rigthBottom)
{
	//１枚目の三角形
	vertexData_[0].position = { leftTop.x,rigthBottom.y,0.0f,1.0f };//左下
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { leftTop.x,leftTop.y,0.0f,1.0f };//左上
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].position = { rigthBottom.x,rigthBottom.y,0.0f,1.0f };//右下
	vertexData_[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexData_[3].position = { rigthBottom.x,leftTop.y,0.0f,1.0f };//右上
	vertexData_[3].texcoord = { 1.0f,0.0f };
}

