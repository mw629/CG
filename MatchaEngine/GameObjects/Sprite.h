#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "../Common/VariableTypes.h"

class Sprite
{
private:

	Transform transform_;


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_ = nullptr;

	//インデックスバッファ用
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t* indexData_ = nullptr;

public:
	Sprite();

	void CreateVertexData(ID3D12Device* device);

	void CreateWVP(ID3D12Device* device);

	void CreateSprite(ID3D12Device* device);

	void SetWvp();

	void SetTrandform(Transform transform);


	D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &vertexBufferView_; }
	ID3D12Resource* GetVertexResource() { return vertexResource_.Get(); }
	D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() { return &indexBufferView_; }


};


