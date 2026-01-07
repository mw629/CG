#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "VariableTypes.h"
#include "Texture.h"
#include "MaterialFactory.h"
#include "RenderState.h"

class Object3DBase
{
protected:

	Transform transform_{};

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

public:

	virtual ~Object3DBase();

	static void SetObjectResource(ID3D12Device* device,Vector2 ClientSize);

	virtual void CreateVertexData() = 0;
	virtual void CreateWVP();
	virtual void CreateObject();

	virtual void SettingWvp(Matrix4x4 viewMatrix);


	void SetTransform(Transform transform) { transform_ = transform_; }
	void SetLighting(bool isActiv) { material_.get()->SetMaterialLighting(isActiv); }
	void SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) { textureSrvHandleGPU_ = textureSrvHandleGPU; }

	//getter
	Transform GetTransform() const { return transform_; }
	MaterialFactory* GetMatrial() { return material_.get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU()const { return textureSrvHandleGPU_; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetWvpDataResource() { return wvpDataResource_.Get(); }
	TransformationMatrix* GetWvpData() { return wvpData_; }

};

