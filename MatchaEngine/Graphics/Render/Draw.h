#pragma once  
#include "../Core/VariableTypes.h"  
#include "Math/Calculation.h"  
#include "PSO/GraphicsPipelineState.h"
#include <d3d12.h>  
#include <cstdint>  

#include "ObjectBase.h"

#include "Model.h"
#include "EffectDefinition.h"
#include "Sprite.h"
#include "Cube.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Line.h"
#include "Grid.h"
#include "Camera.h"

#include "CharacterAnimator.h"

#include "LightManager.h"

class Draw {
public:

	static void Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState, 
		LightManager* lightManager);

	static void SetCamera(Camera* setcamera);
	static void SetEnvironmentTexture(int textureHandel);

	static void preDraw(ShaderName shader, BlendMode blend);

	static void DrawObj(ObjectBase *obj);

	static void DrawAnimation(CharacterAnimator* obj);

	static void DrawModel(Model* model);

   static void DrawParticle(EffectDefinition* particle);

	static void DrawSprite(Sprite* sprite);

	static void DrawSphere(Sphere* sphere);

	static void DrawTriangle(Triangle* triangle);

	static void DrawLine(Line* line);

	static void DrawGrid(Grid* grid);

	static void DrawPostEffect(D3D12_GPU_DESCRIPTOR_HANDLE textureHandle, ShaderName shader = "CopyShader", class PostEffect* postEffect = nullptr, D3D12_GPU_DESCRIPTOR_HANDLE depthTextureHandle = {0});

};
