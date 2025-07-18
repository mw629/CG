#pragma once
#include <fstream>

///自作エンジン///

//Common
#include "Common/CommandContext.h"

//Core
#include "Core/LogHandler.h"
#include "Core/VariableTypes.h"
#include "Core/WindowConfig.h"

//GameObjeects
#include "GameObjects/DebugCamera.h"
#include "GameObjects/MaterialFactory.h"
#include "GameObjects/Model.h"
#include "GameObjects/Sphere.h"
#include "GameObjects/Sprite.h"
#include "GameObjects/Triangle.h"
#include "GameObjects/Line.h"
#include "GameObjects/Grid.h"

//Graphics

#include "Graphics/DepthStencil.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/DirectinalLight.h"
#include "Graphics/Draw.h"
#include "Graphics/GpuSyncManager.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/RenderTargetView.h"
#include "Graphics/ResourceBarrierHelper.h"
#include "Graphics/SwapChain.h"
#include "Graphics/ViewportScissor.h"
//PSO
#include "Graphics/PSO/BlendState.h"
#include "Graphics/PSO/DepthStencilState.h"
#include "Graphics/PSO/DirectXShaderCompiler.h"
#include "Graphics/PSO/GraphicsPipelineState.h"
#include "Graphics/PSO/InputLayout.h"
#include "Graphics/PSO/RasterizerState.h"
#include "Graphics/PSO/RootSignature.h"
#include "Graphics/PSO/RootParameter.h"
#include "Graphics/PSO/Sampler.h"
#include "Graphics/PSO/ShaderCompile.h"

//Input

#include "Input/Input.h"

//Math

#include "Math/Calculation.h"

//Resource

#include "Resource/Audio.h"
#include "Resource/Load.h"
#include "Resource/Texture.h"
#include "Resource/TextureLoader.h"


class Engine
{
private:

	HRESULT hr_;

	int32_t kClientWidth_;
	int32_t kClientHeight_;

	std::ofstream logStream;

	WindowConfig window;

	std::unique_ptr<GraphicsDevice> graphics;
	std::unique_ptr<CommandContext> command;
	std::unique_ptr<SwapChain> swapChain;
	std::unique_ptr<DescriptorHeap> descriptorHeap;
	std::unique_ptr<RenderTargetView> renderTargetView;
	std::unique_ptr<ViewportScissor> viewportScissor;

	std::unique_ptr<Input> input;

	std::unique_ptr<DebugCamera> debudCamera;
	std::unique_ptr<DepthStencil> depthStencil;

	std::unique_ptr<Draw> draw;

	GpuSyncManager gpuSyncManager;

	std::unique_ptr<ResourceBarrierHelper> resourceBarrierHelper;

	DirectXShaderCompiler directXShaderCompiler;
	std::unique_ptr<RootSignature> rootSignature;
	std::unique_ptr<RootParameter> rootParameter;
	std::unique_ptr<Sampler> sampler;
	std::unique_ptr<InputLayout> inputLayout;
	std::unique_ptr<BlendState> blendState;
	std::unique_ptr<RasterizerState> rasterizerState;
	std::unique_ptr<ShaderCompile> shaderCompile;
	std::unique_ptr<DepthStencilState> depthStencilState;
	std::unique_ptr<GraphicsPipelineState> graphicsPipelineState;

	std::unique_ptr<RootSignature> linerootSignature;
	std::unique_ptr<RootParameter> linerootParameter;
	std::unique_ptr<InputLayout> lineinputLayout;
	std::unique_ptr<ShaderCompile> lineshaderCompile;
	std::unique_ptr<GraphicsPipelineState> lineGraphicsPipeState;

	std::unique_ptr<DirectinalLight> directinalLight;

public:

	~Engine();
	Engine(int32_t kClientWidth, int32_t kClientHeight);

	void Setting();

	void PreDraw();

	void PostDraw();

	void EndFrame();


};

