#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"
#include "Texture.h"
#include "MaterialFactory.h"
#include "RenderState.h"

class ObjectBase
{
protected:
	// 派生クラスからアクセス可能な静的メンバー
	static float kClientWidth_;
	static float kClientHeight_;

	Transform transform_;

	//objectResource


	//マテリアルデータ
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	std::unique_ptr<MaterialFactory> material_{};
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_{};

	//頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpDataResource_;
	TransformationMatrix* wvpData_ = nullptr;
	int vertexSize_ = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_ = nullptr;


public:

	virtual ~ObjectBase();

	virtual void ImGui();

	static void SetObjectResource(Vector2 ClientSize);

	virtual void CreateVertexData() = 0;
	virtual void CreateWVP();
	void CreateIndexResource();

	virtual void CreateObject();

	virtual void SettingWvp(Matrix4x4 viewMatrix);


	void SetTransform(Transform transform) { transform_ = transform; }  // バグ修正: transform_ = transform_ → transform_ = transform
	void SetLighting(bool isActive) { material_.get()->SetMaterialLighting(isActive); }
	void SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) { textureSrvHandleGPU_ = textureSrvHandleGPU; }

	//getter
	Transform GetTransform() const { return transform_; }
	MaterialFactory* GetMartial() { return material_.get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetWvpDataResource() { return wvpDataResource_.Get(); }
	int GetVertexSize() { return vertexSize_;}
	TransformationMatrix* GetWvpData() { return wvpData_; }

private:
	ShaderName shader_ = ShaderName::ObjectShader;
	BlendMode blend_ = BlendMode::kBlendModeNone;
public:

	void SetShader(ShaderName shader) { shader_ = shader; }
	void SetBlend(BlendMode blend) { blend_ = blend; }

	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }


};

