#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "Core/VariableTypes.h"
#include "Resource/Texture.h"
#include "MaterialFactory.h"
#include "../Graphics/PSO/RenderState.h"

class Particle
{
private:

	ModelData modelData_{};
	std::unique_ptr<MaterialFactory> material_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	TransformationMatrix* instancingData_=nullptr;
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc_{};

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

	int particleNum_;
	std::vector<Transform> transform_;


public:

	static void SetDevice(ID3D12Device* device);
	static void SetScreenSize(Vector2 screenSize);
	static void SetDescriptorHeap(DescriptorHeap* descriptorHeap);

	void Initialize(std::vector<Transform> transform);

	void CreateVertexData();
	void CreateWVP();
	void CreateSRV();

	void SettingWvp(Matrix4x4 viewMatrix);
	void SetTransform(std::vector<Transform> transform);

	void CreateParticle();

	ModelData GetModelData() { return modelData_; }
	MaterialFactory* GetMatrial() { return material_.get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetInstancingResource() { return instancingResource_.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleCPU() { return instancingSrvHandleCPU_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetInstancingSrvHandleGPU() { return instancingSrvHandleGPU_; }

	int GetParticleNum() { return particleNum_; }

private:
	ShaderName shader_ = ShaderName::ParticleShader;
	BlendMode blend_ = BlendMode::kBlendModeNone;
public:
	void SetBlend(BlendMode blend) { blend_ = blend; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }


};

