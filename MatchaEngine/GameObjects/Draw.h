#pragma once  
#include "Common/VariableTypes.h"  
#include "Math/Calculation.h"  
#include <d3d12.h>  
#include <cstdint>  
#include "Model.h"

class Draw {
private:

	ID3D12GraphicsCommandList* commandList_;

	ID3D12Resource* vertexResource;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	VertexData* vertexData = nullptr;
	ID3D12Resource* wvpResource;
	Matrix4x4* wvpData;

	
	//インデックスバッファ用
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_ = nullptr;

public:

	Draw(ID3D12GraphicsCommandList* commandList);
	~Draw();

	void Initialize();

	void CreateIndexBuffer(ID3D12Device* device);

	void DrawObj(Model* model);

	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }

};
