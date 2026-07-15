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
#include "LineRenderer.h"
#include "Camera.h"

#include "CharacterAnimator.h"

#include "LightManager.h"

class Draw {
public:

	void Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState, 
		LightManager* lightManager);

	void SetCamera(Camera* setcamera);
	void SetEnvironmentTexture(int textureHandel);

	void preDraw(ShaderName shader, BlendMode blend);

	void DrawObj(ObjectBase *obj);

	void DrawAnimation(CharacterAnimator* obj);

	void DrawModel(Model* model);

   void DrawParticle(EffectDefinition* particle);

	void DrawSprite(Sprite* sprite);

	void DrawSphere(Sphere* sphere);

	void DrawTriangle(Triangle* triangle);

	void DrawLine(Line* line);

	void DrawGrid(Grid* grid);

	void DrawAllLines(LineRenderer* lineRenderer, bool depthTest = true);


	void DrawPostEffect(D3D12_GPU_DESCRIPTOR_HANDLE textureHandle, ShaderName shader = "CopyShader", class PostEffect* postEffect = nullptr, D3D12_GPU_DESCRIPTOR_HANDLE depthTextureHandle = {0});

private:
	void SetCBV(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS address);
	void SetSRV(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS address);
	void SetTable(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_DESCRIPTOR_HANDLE handle);

	ID3D12GraphicsCommandList* commandList_{};
	GraphicsPipelineState* graphicsPipelineState_{};
	LightManager* lightManager_{};
	Camera* camera_{};
	D3D12_GPU_DESCRIPTOR_HANDLE environmentTextureSrvHandleGPU_{};
};
