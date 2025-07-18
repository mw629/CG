#pragma once
#include <wrl.h>
#include "../Core/VariableTypes.h" 
#include "MaterialFactory.h"      
#include <d3dx12.h>                

class Grid {

private:

	int kSubdivision_ = 60;

	int index = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	LineVertexData* vertexData_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	LineTransformationMatrix* wvpData_=nullptr;


public:

	~Grid();


	void CreateVertexData(ID3D12Device* device);

	void CreateWVP(ID3D12Device* device);

	void CreateGrid(ID3D12Device* device);

	void SetWvp(Matrix4x4 viewMatrix);

	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource()const { return wvpResource_.Get(); }

	int GetSubdivision()const {return kSubdivision_;}

};