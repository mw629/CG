#pragma once  
#include "../Core/VariableTypes.h"  
#include "Math/Calculation.h"  
#include <d3d12.h>  
#include <cstdint>  
#include "../GameObjects/Model.h"
#include "../GameObjects/Sprite.h"
#include "../GameObjects/Sphere.h"
#include "../GameObjects/Triangle.h"
#include "../GameObjects/Line.h"
#include "../GameObjects/Grid.h"

class Draw {
public:

	static void Initialize(ID3D12GraphicsCommandList* commandList);

	void DrawObj(Model* model);

	void DrawSprite(Sprite* sprite);

	void DrawShpere(Sphere* sphere);

	void DrawTriangle(Triangle* triangle);

	void DrawLine(Line* line);

	void DrawGrid(Grid* grid);
	
};
