#define NOMINMAX

#include "EffectDefinition.h"
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

int EffectDefinition::DescriptorNum = 5;

void EffectDefinition::SetDevice(ID3D12Device* device)
{
	device_ = device;
}

void EffectDefinition::SetScreenSize(Vector2 screenSize)
{
	kClientWidth = screenSize.x;
	kClientHeight = screenSize.y;
}

void EffectDefinition::SetDescriptorHeap(DescriptorHeap* descriptorHeap)
{
	descriptorHeap_ = descriptorHeap;
}

void EffectDefinition::Initialize()
{

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	textureSrvHandleGPU_ = texture.get()->TextureData(texture.get()->CreateTexture("resources/circle.png"));

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMartial(false);
	CreateParticle();
}

void EffectDefinition::Initialize(int TextureHandle)
{
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	textureSrvHandleGPU_ = texture.get()->TextureData(TextureHandle);

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMartial();
	CreateParticle();
}

void EffectDefinition::CreateVertexData()
{
	std::vector<VertexData> vertices;

	vertices.push_back({ .position = { -1.0f,  1.0f, 0.0f, 1.0f }, .texcoord = {0.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} }); // 左上
	vertices.push_back({ .position = {  1.0f,  1.0f, 0.0f, 1.0f }, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} }); // 右上
	vertices.push_back({ .position = { -1.0f, -1.0f, 0.0f, 1.0f }, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} }); // 左下

	vertices.push_back({ .position = { -1.0f, -1.0f, 0.0f, 1.0f }, .texcoord = {0.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} }); // 左下
	vertices.push_back({ .position = {  1.0f,  1.0f, 0.0f, 1.0f }, .texcoord = {1.0f,0.0f}, .normal = {0.0f,0.0f,1.0f} }); // 右上
	vertices.push_back({ .position = {  1.0f, -1.0f, 0.0f, 1.0f }, .texcoord = {1.0f,1.0f}, .normal = {0.0f,0.0f,1.0f} }); // 右下

	//頂点リソースを作る
	vertexResource_ = GraphicsDevice::CreateBufferResource(sizeof(VertexData) * 6);
	//頂点バッファービューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * 6);
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, vertices.data(), sizeof(VertexData) * 6);
}


void EffectDefinition::CreateWVP()
{

	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	instancingResource_ = GraphicsDevice::CreateBufferResource(sizeof(ParticleForGPU) * effectDefinitionMaxNum_);
	//データを書き込む
	//書き込むためのアドレスを取得
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
	//単位行列をかきこんでおく
	for (int i = 0; i < effectDefinitionMaxNum_; i++) {
		instancingData_[i].WVP = IdentityMatrix();
		instancingData_[i].World = IdentityMatrix();
		instancingData_[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void EffectDefinition::CreateSRV()
{
	instancingSrvDesc_.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc_.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc_.Buffer.FirstElement = 0;
	instancingSrvDesc_.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc_.Buffer.NumElements = effectDefinitionMaxNum_;
	instancingSrvDesc_.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instancingSrvHandleCPU_ = GetCPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV());
	instancingSrvHandleGPU_ = GetGPUDescriptorHandle(descriptorHeap_->GetSrvDescriptorHeap(), descriptorHeap_->GetDescriptorSizeSRV());

	device_->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc_, instancingSrvHandleCPU_);
}



void EffectDefinition::CreateParticle()
{
	CreateVertexData();
	CreateWVP();
	CreateSRV();
	DescriptorNum++;
}

void EffectDefinition::DeleteParticle(int ParticleNum)
{
	int index = 0;
	for (auto it = effectDefinitionData_.begin(); it != effectDefinitionData_.end(); ) {
		if (index == ParticleNum) {
			it = effectDefinitionData_.erase(it);
			return;
		}
		++it;
		++index;
	}
}


void EffectDefinition::SettingWvp(Matrix4x4 viewMatrix)
{
    Matrix4x4 billboard = viewMatrix;
    billboard = Inverse(billboard);
    billboard.m[3][0] = billboard.m[3][1] = billboard.m[3][2] = 0.0f;

    Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);

    auto particleIter = effectDefinitionData_.begin();
    auto instancingIter = instancingData_;

    int i = 0;
    while (particleIter != effectDefinitionData_.end() && i < effectDefinitionMaxNum_) {
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
	effectDefinitionNum_ = i;
}

void EffectDefinition::SetData(std::list<EffectDefinitionData> effectDefinitionData)
{
	effectDefinitionData_ = std::move(effectDefinitionData);
}

void EffectDefinition::Updata(Matrix4x4 viewMatrix, std::list<EffectDefinitionData> effectDefinitionData)
{
    SetData(effectDefinitionData);

    // 寿命切れを削除
    for (auto it = effectDefinitionData_.begin(); it != effectDefinitionData_.end(); ) {
        if (it->lifeTime <= it->currentTime) {
            it = effectDefinitionData_.erase(it);
        } else {
            ++it;
        }
    }

    // 一度だけWVPを更新（最大particleMaxNum_まで）
    SettingWvp(viewMatrix);
}

