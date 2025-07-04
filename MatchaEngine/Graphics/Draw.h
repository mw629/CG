#pragma once  
#include "../Core/VariableTypes.h"  
#include "Math/Calculation.h"  
#include <d3d12.h>  
#include <cstdint>  
#include "../GameObjects/Model.h"
#include "../GameObjects/Sprite.h"
#include "../GameObjects/Sphere.h"

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
