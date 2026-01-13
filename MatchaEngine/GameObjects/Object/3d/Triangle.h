#pragma once
#include "VariableTypes.h"
#include "MaterialFactory.h"
#include "RenderState.h"

class Triangle
{
private:

	Transform transform_{};
	Vector4 vertex_[3];
	MaterialFactory* material_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_{};
	TransformationMatrix* wvpData_{};


public:
	~Triangle();

	static void SetScreenSize(Vector2 screenSize);


	void Initialize( D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);
	
	void CreateVertexData();

	void CreateWVP();

	void CreateTriangle();

	void SettingWvp(Matrix4x4 viewMatrix);


	void SetShape();
	void SetTrandform(Transform transform);
	void SetVertex(Vector4 vertex[3]);
	void SetMaterialLighting(bool isActiv) { material_->SetMaterialLighting(isActiv); }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource()const { return wvpResource_.Get(); }
	MaterialFactory* GetMatrial()const { return material_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }

private:
	ShaderName shader_ = ShaderName::ObjectShader;
	BlendMode blend_ = BlendMode::kBlendModeNone;
public:
	BlendMode SetBlend(BlendMode blend) { blend_ = blend; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }


};

