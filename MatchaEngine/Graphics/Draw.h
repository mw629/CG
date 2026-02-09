#pragma once  
#include "../Core/VariableTypes.h"  
#include "Math/Calculation.h"  
#include "PSO/GraphicsPipelineState.h"
#include <d3d12.h>  
#include <cstdint>  

#include "ObjectBase.h"

#include "Model.h"
#include "Particle.h"
#include "Sprite.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Line.h"
#include "Grid.h"
#include "Camera.h"

#include "CharacterAnimator.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Draw {
public:

	static void Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState, 
		DirectionalLight* directionalLight, PointLight *pointLight, SpotLight* spotLight);

	static void SetCamera(Camera* setcamera);

	static void preDraw(ShaderName shader, BlendMode blend);

	static void DrawObj(ObjectBase *obj);

	static void DrawAnimtion(CharacterAnimator* obj);

	static void DrawModel(Model* model, Camera* camera);

	static void DrawParticle(Particle* particle);

	static void DrawSprite(Sprite* sprite, Camera* camera);

	static void DrawShpere(Sphere* sphere, Camera* camera);

	static void DrawTriangle(Triangle* triangle, Camera* camera);

	static void DrawLine(Line* line);

	static void DrawGrid(Grid* grid);

};
