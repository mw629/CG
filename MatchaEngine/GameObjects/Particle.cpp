#define NOMINMAX

#include "Particle.h"
#include <Math/Calculation.h>
#include "Graphics/GraphicsDevice.h"
#include "Sprite.h"
#include <algorithm>
#include <d3d12.h>
#include <d3dx12.h> 
#include "../Graphics/DescriptorHeap.h"
#include <Resource/Load.h>

namespace {
	ID3D12Device* device_;
	float kClientWidth;
	float kClientHeight;

	DescriptorHeap* descriptorHeap_;
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

void Particle::SetDescriptorHeap(DescriptorHeap* descriptorHeap)
{
	descriptorHeap_ = descriptorHeap;
}

void Particle::Initialize(std::vector<ParticleData> data)
{
	particleData_ = data;
	particleNum_ = data.size();

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
	vertexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(VertexData) * 6);
	//頂点バッファービューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * 6);
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * 6);
}


void Particle::CreateWVP()
{

	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	instancingResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(ParticleForGPU) * particleNum_);
	//データを書き込む
	//書き込むためのアドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	//単位行列をかきこんでおく
	for (int i = 0; i < particleNum_; i++) {
		instancingData_[i].WVP = IdentityMatrix();
		instancingData_[i].World = IdentityMatrix();
		instancingData_[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void Particle::CreateSRV()
{
	instancingSrvDesc_.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc_.Buffer.FirstElement = 0;
	instancingSrvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc_.Buffer.NumElements = particleNum_;
	instancingSrvDesc_.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV(), 3);
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV(), 3);

	device_->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc_, instancingSrvHandleCPU_);
}



void Particle::CreateParticle()
{
	CreateVertexData();
	CreateWVP();
	CreateSRV();
}

void Particle::SettingWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 billboard = viewMatrix;
	billboard = Inverse(billboard);
	billboard.m[3][0] = billboard.m[3][1] = billboard.m[3][2] = 0.0f; // 平行移動成分消す
	

	Matrix4x4 worldMatrixObj;

	for (int i = 0; i < particleNum_; i++) {
		Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
		if (!isBillboard_) {
			worldMatrixObj = MakeAffineMatrix(particleData_[i].transform.translate, particleData_[i].transform.scale, particleData_[i].transform.rotate);
		}
		else {
			worldMatrixObj = MultiplyMatrix4x4(MakeAffineMatrix(particleData_[i].transform.translate, particleData_[i].transform.scale, particleData_[i].transform.rotate), billboard);
		}
		Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));
		instancingData_[i] = { worldViewProjectionMatrixObj,worldMatrixObj };
		instancingData_[i].color = particleData_[i].color;

	}
}

void Particle::SetData(std::vector<ParticleData> particleData)
{
	particleData_ = particleData;
}

void Particle::Updata(Matrix4x4 viewMatrix, std::vector<ParticleData> particleData)
{
	SetData(particleData);
	SettingWvp(viewMatrix);
}
