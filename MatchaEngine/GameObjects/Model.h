#pragma once
#include <wrl.h>
#include <d3dx12.h>
#include "Common/VariableTypes.h"
#include "Resource/Texture.h"
#include "Matrial.h"


class Model
{
private:
	ModelData modelData_;
	Transform transform_;
	Matrial matrial_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpDataResource_;
	TransformationMatrix* wvpData_ = nullptr;

	bool isAlive = true;
	int kClientWidth = 1280;
	int kClientHeight = 720;
public:


	Model(ModelData modelData, Matrial matrial);

	void CreateVertexData(ID3D12Device* device);
	void CreateWVP(ID3D12Device* device);

	//void SetTexture(ID3D12Device* device);



	void SetWvp(Transform camera);
	void SetTransform(Transform transform);

	void CreateModel(ID3D12Device* device);


	Transform GetTransform() { return transform_; }
	void SetPos(Vector3 velocity) { transform_.translate = velocity; }


	ModelData GetModelData() { return modelData_; }

	Matrial GetMatrial() { return matrial_; }

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }

	ID3D12Resource* GetWvpDataResource() { return wvpDataResource_.Get(); }
	TransformationMatrix* GetWvpData() { return wvpData_; }



	void SetAlive(bool flag) { isAlive = flag; }
	bool GetAlive() { return isAlive; }
};

