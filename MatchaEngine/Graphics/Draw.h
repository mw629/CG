#pragma once  
#include "../Core/VariableTypes.h"  
#include "Math/Calculation.h"  
#include "PSO/GraphicsPipelineState.h"
#include <d3d12.h>  
#include <cstdint>  
#include "../GameObjects/Model.h"
#include "../GameObjects/Sprite.h"
#include "../GameObjects/Sphere.h"
#include "../GameObjects/Triangle.h"
#include "../GameObjects/Line.h"
#include "../GameObjects/Grid.h"
#include "DirectinalLight.h"

class Draw {
public:

	static void Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState, DirectinalLight* directinalLight);

	static void preDraw(ShaderName shader, BlendMode blend);

	static void DrawModel(Model* model);

	static void DrawSprite(Sprite* sprite);

	static void DrawShpere(Sphere* sphere);

	static void DrawTriangle(Triangle* triangle);

	static void DrawLine(Line* line);

	static void DrawGrid(Grid* grid);

};
