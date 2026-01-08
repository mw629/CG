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

int Particle::DescriptorNum = 5;

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

void Particle::Initialize()
{

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	textureSrvHandleGPU_ = texture.get()->TextureData(texture.get()->CreateTexture("resources/circle.png"));

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMatrial(device_, false);
	CreateParticle();
}

void Particle::Initialize(int TextureHandle)
{
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	textureSrvHandleGPU_ = texture.get()->TextureData(TextureHandle);

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMatrial(device_);
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
	instancingResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(ParticleForGPU) * particleMaxNum_);
	//データを書き込む
	//書き込むためのアドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	//単位行列をかきこんでおく
	for (int i = 0; i < particleMaxNum_; i++) {
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
	instancingSrvDesc_.Buffer.NumElements = particleMaxNum_;
	instancingSrvDesc_.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV());
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV());

	device_->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc_, instancingSrvHandleCPU_);
}



void Particle::CreateParticle()
{
	CreateVertexData();
	CreateWVP();
	CreateSRV();
	DescriptorNum++;
}

void Particle::DeleteParticle(int ParticleNum)
{
	int index = 0;
	for (auto it = particleData_.begin(); it != particleData_.end(); ) {
		if (index == ParticleNum) {
			it = particleData_.erase(it);
			return;
		}
		++it;
		++index;
	}
}


void Particle::SettingWvp(Matrix4x4 viewMatrix)
{
    Matrix4x4 billboard = viewMatrix;
    billboard = Inverse(billboard);
    billboard.m[3][0] = billboard.m[3][1] = billboard.m[3][2] = 0.0f;

    Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);

    auto particleIter = particleData_.begin();
    auto instancingIter = instancingData_;

    int i = 0;
    while (particleIter != particleData_.end() && i < particleMaxNum_) {
        Matrix4x4 worldMatrixObj;
        if (!isBillboard_) {
            worldMatrixObj = MakeAffineMatrix(particleIter->transform.translate, particleIter->transform.scale, particleIter->transform.rotate);
        } else {
            worldMatrixObj = MultiplyMatrix4x4(MakeAffineMatrix(particleIter->transform.translate, particleIter->transform.scale, particleIter->transform.rotate), billboard);
        }
        Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));

        instancingIter[i].WVP = worldViewProjectionMatrixObj;
        instancingIter[i].World = worldMatrixObj;
        instancingIter[i].color = particleIter->color;

        ++particleIter;
        ++i;
    }

    // 実際に書き込んだ要素数を反映
    particleNum_ = i;
}

void Particle::SetData(std::list<ParticleData> particleData)
{
	particleData_ = std::move(particleData);
}

void Particle::Updata(Matrix4x4 viewMatrix, std::list<ParticleData> particleData)
{
    SetData(particleData);

    // 寿命切れを削除
    for (auto it = particleData_.begin(); it != particleData_.end(); ) {
        if (it->lifeTime <= it->currentTime) {
            it = particleData_.erase(it);
        } else {
            ++it;
        }
    }

    // 一度だけWVPを更新（最大particleMaxNum_まで）
    SettingWvp(viewMatrix);
}

