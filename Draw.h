#pragma once
#include "VariableTypes.h"
#include "Calculation.h"
#include <d3d12.h>
#include <cstdint>

class Draw
{
private:

	int kClientWidth = 1280;
	int kClientHeight = 720;

	float pi = 3.14f;
	uint32_t kSubdivision = 16;

public:

	

	void CreateTriangle(ID3D12Device* device);
	void DrawTriangle(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,Transform camraTransform, Transform transform);

};

