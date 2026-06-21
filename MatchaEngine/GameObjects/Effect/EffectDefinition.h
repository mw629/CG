#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"
#include "Texture.h"
#include "MaterialFactory.h"
#include "PipelineState.h"

struct EffectDefinitionData {
	Transform transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	Vector3 velocity = {0.0f,0.0f,0.0f};
	Vector4 color = {1.0f,1.0f,1.0f,1.0f};
	float lifeTime=3.0f;
	float currentTime = 0.0f;
};

enum class EffectShape {
	Plane,
	Cylinder,
	Ring
};

struct EffectShapeData {
	int cylinderDivide = 32;
	float cylinderTopRadius = 1.0f;
	float cylinderBottomRadius = 1.0f;
	float cylinderHeight = 3.0f;

	int ringDivide = 32;
	float ringOuterRadius = 1.0f;
	float ringInnerRadius = 0.8f;
};

class EffectDefinition
{
private:
	EffectShapeData shapeData_;

	ModelData modelData_{};
	std::unique_ptr<MaterialFactory> material_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	ParticleForGPU* instancingData_ = nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc_{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;



	int effectDefinitionMaxNum_ = 10000;
	int effectDefinitionNum_;
	
	std::list<EffectDefinitionData> effectDefinitionData_;

	bool isBillboard_ = true;
	
	EffectShape shape_ = EffectShape::Plane;
	uint32_t vertexSize_ = 6;

	static int DescriptorNum;

public:

	static void SetDevice(ID3D12Device* device);
	static void SetScreenSize(Vector2 screenSize);
	static void SetDescriptorHeap(DescriptorHeap* descriptorHeap);

	void Initialize(EffectShape shape = EffectShape::Plane);
	void Initialize(int TextureHandle, EffectShape shape = EffectShape::Plane);

	void SetTexturePath(const std::string& path);
	void SetShape(EffectShape shape, const EffectShapeData& data);
	void SetShapeData(const EffectShapeData& data);

	void CreateVertexData();
	void CreateWVP();
	void CreateSRV();

	void SettingWvp(Matrix4x4 viewMatrix);
	void SetData(std::list<EffectDefinitionData> effectDefinitionData);

	void Updata(Matrix4x4 viewMatrix, std::list<EffectDefinitionData> effectDefinitionData);

	void CreateParticle();

	void DeleteParticle(int ParticleNum);

	void SetBillboard(bool flag) { isBillboard_ = flag; }

	ModelData GetModelData() { return modelData_; }
	MaterialFactory* GetMartial() { return material_.get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetInstancingResource() { return instancingResource_.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleCPU() { return instancingSrvHandleCPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleGPU() { return instancingSrvHandleGPU_; }

	int GetEffectDefinitionNum() { return effectDefinitionNum_; }
	uint32_t GetVertexSize() const { return vertexSize_; }

	std::list<EffectDefinitionData> GetEffectDefinitionData() { return effectDefinitionData_; }

private:
	ShaderName shader_ = "ParticleShader";
	BlendMode blend_ = BlendMode::kBlendModeAdd;
public:
	void SetBlend(BlendMode blend) { blend_ = blend; }
	void SetShader(ShaderName shader) { shader_ = shader; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }


};
