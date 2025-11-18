#define NOMINMAX

#include "Particle.h"
#include <Math/Calculation.h>
#include "Graphics/GraphicsDevice.h"
#include "Sprite.h"
#include <algorithm>

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

void Particle::Initialize(std::vector<Transform> transform)
{
	transform_ = transform;
	particleNum_ = transform.size();

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	textureSrvHandleGPU_ = texture.get()->TextureData(texture.get()->CreateTexture("resources/uvChecker.png"));

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMatrial(device_, false);
	CreateParticle();
}

void Particle::CreateVertexData()
{
	modelData_.vertices.push_back({ .position = { -1.0f,  1.0f, 0.0f, 1.0f }, .texcoord = {0.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} }); // 左上
	modelData_.vertices.push_back({ .position = {  1.0f,  1.0f, 0.0f, 1.0f }, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} }); // 右上
	modelData_.vertices.push_back({ .position = { -1.0f, -1.0f, 0.0f, 1.0f }, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} }); // 左下

	modelData_.vertices.push_back({ .position = { -1.0f, -1.0f, 0.0f, 1.0f }, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} }); // 左下
	modelData_.vertices.push_back({ .position = {  1.0f,  1.0f, 0.0f, 1.0f }, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} }); // 右上
	modelData_.vertices.push_back({ .position = {  1.0f, -1.0f, 0.0f, 1.0f }, .texcoord = {1.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} }); // 右下

	//頂点リソースを作る
	vertexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());
	//頂点バッファービューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}


void Particle::CreateWVP()
{
	
		Microsoft::WRL::ComPtr<ID3D12Resource>wvpDataResource;
		TransformationMatrix* wvpData = nullptr;
		//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
		wvpDataResource = GraphicsDevice::CreateBufferResource(device_, sizeof(TransformationMatrix)* particleNum_);
		//データを書き込む
		//書き込むためのアドレスを取得
		wvpDataResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
		//単位行列をかきこんでおく
		wvpData->WVP = IdentityMatrix();
		wvpData->World = IdentityMatrix();
		
		wvpDataResource_.push_back(wvpDataResource);
		wvpData_.push_back(wvpData);
	}
}


void Particle::CreateParticle()
{
	CreateVertexData();
	CreateWVP();
}

void Particle::SettingWvp(Matrix4x4 viewMatrix)
{
	for (int i = 0; i < particleNum_; i++) {
		Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
		Matrix4x4 worldMatrixObj = MakeAffineMatrix(transform_[i].translate, transform_[i].scale, transform_[i].rotate);
		Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));
		*wvpData_[i] = {worldViewProjectionMatrixObj,worldMatrixObj};
	}
}

void Particle::SetTransform(std::vector<Transform> transform)
{
	size_t count = std::min(transform_.size(), transform.size());
	for (size_t i = 0; i < count; i++) {
		transform_[i] = transform[i];
	}
}