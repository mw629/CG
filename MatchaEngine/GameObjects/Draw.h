#pragma once  
#include "Common/VariableTypes.h"  
#include "Math/Calculation.h"  
#include <d3d12.h>  
#include <cstdint>  
#include "Model.h"
#include "Sprite.h"
#include "Sphere.h"

class Draw {
private:

	ID3D12GraphicsCommandList* commandList_;

	ID3D12Resource* vertexResource;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	VertexData* vertexData = nullptr;
	ID3D12Resource* wvpResource;
	Matrix4x4* wvpData;

	


public:

	Draw(ID3D12GraphicsCommandList* commandList);
	~Draw();

	void Initialize();

	void DrawObj(Model* model);

	void DrawSprite(Sprite* sprite);

	void DrawShpere(Sphere* sphere);
	
};
