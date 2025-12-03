#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"
#include "Texture.h"
#include "MaterialFactory.h"
#include "RenderState.h"


class Model
{
private:
	ModelData modelData_{};
	Transform transform_{};
	std::unique_ptr<MaterialFactory> material_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpDataResource_;
	TransformationMatrix* wvpData_ = nullptr;


	bool isAlive = true;



public:
	~Model();

	void ImGui();

	static void SetDevice(ID3D12Device* device);
	static void SetScreenSize(Vector2 screenSize);

	void Initialize(ModelData modelData);

	void CreateVertexData();
	void CreateWVP();

	
	void SettingWvp(Matrix4x4 viewMatrix);
	void SetTransform(Transform transform);

	void CreateModel();

	
	
	void SetPos(Vector3 velocity) { transform_.translate = velocity; }
	void SetAlive(bool flag) { isAlive = flag; }
	void SetMaterialLighting(bool isActiv) { material_.get()->SetMaterialLighting(isActiv); }
	void SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) { textureSrvHandleGPU_ = textureSrvHandleGPU; }

	//getter
	bool GetAlive() const { return isAlive; }
	Transform GetTransform() const { return transform_; }
	
	ModelData GetModelData() { return modelData_; }
	MaterialFactory* GetMatrial() { return material_.get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }


	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetWvpDataResource() { return wvpDataResource_.Get(); }
	TransformationMatrix* GetWvpData() { return wvpData_; }

private:
	ShaderName shader_ = ShaderName::ObjectShader;
	BlendMode blend_ = BlendMode::kBlendModeNone;
public:
	void SetBlend(BlendMode blend) { blend_ = blend; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }
	
};

