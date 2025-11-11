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
	MaterialFactory* material_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource> >wvpDataResource_;
	std::vector<TransformationMatrix*> wvpData_;

	int particleNum_;
	std::vector<Transform> transform_;


public:

	static void SetDevice(ID3D12Device* device);
	static void SetScreenSize(Vector2 screenSize);

	void Initialize(std::vector<Transform> transform);

	void CreateVertexData();
	void CreateWVP();

	void SettingWvp(Matrix4x4 viewMatrix);
	void SetTransform(std::vector<Transform> transform);

	void CreateParticle();

	ModelData GetModelData() { return modelData_; }
	MaterialFactory* GetMatrial() { return material_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetWvpDataResource(int particleNum) { return wvpDataResource_[particleNum].Get(); }
	TransformationMatrix* GetWvpData(int particleNum) { return wvpData_[particleNum]; }

	int GetParticle() { return particleNum_; }

private:
	ShaderName shader_ = ShaderName::ObjectShader;
	BlendMode blend_ = BlendMode::kBlendModeNone;
public:
	void SetBlend(BlendMode blend) { blend_ = blend; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }


};

