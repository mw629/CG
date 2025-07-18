#pragma once
#include <wrl.h>
#include "../Core/VariableTypes.h" 
#include "MaterialFactory.h"      
#include <d3dx12.h>                

class Line {

private:

	Transform transform_{};
	LineVertexData vertex_[2]{};
	
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	LineVertexData* vertexData_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	LineTransformationMatrix* wvpData_=nullptr;


public:

	~Line();


	void CreateVertexData(ID3D12Device* device);

	void CreateWVP(ID3D12Device* device);

	void CreateLine(ID3D12Device* device);

	void SetWvp(Matrix4x4 viewMatrix);

	void SetTrandform(Transform transform);

	void SetVertex(LineVertexData vertex[2]);


	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource() { return wvpResource_.Get(); }


};