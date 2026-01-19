#pragma once
#include <fstream>
#include <chrono>

///自作エンジン///

//Common
#include "CommandContext.h"

//Core
#include "LogHandler.h"
#include "VariableTypes.h"
#include "WindowConfig.h"

//GameObjects

//Light

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

//Camera

#include "Camera.h"
#include "DebugCamera.h"


#include "MaterialFactory.h"
#include "Model.h"
#include "CharacterAnimator.h"
#include "Particle.h"
#include "Sphere.h"
#include "Sprite.h"
#include "Triangle.h"
#include "Line.h"
#include "Grid.h"

//Graphics

#include "DepthStencil.h"
#include "DescriptorHeap.h"
#include "Draw.h"
#include "GpuSyncManager.h"
#include "GraphicsDevice.h"
#include "RenderTargetView.h"
#include "ResourceBarrierHelper.h"
#include "SwapChain.h"
#include "ViewportScissor.h"
//PSO
#include "BlendState.h"
#include "DepthStencilState.h"
#include "DirectXShaderCompiler.h"
#include "GraphicsPipelineState.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "RootSignature.h"
#include "RootParameter.h"
#include "Sampler.h"
#include "ShaderCompile.h"

//Input

#include "Input.h"
#include "GamePadInput.h"

//Math

#include "Calculation.h"

//Resource

#include "Audio.h"
#include "Load.h"
#include "Texture.h"
#include "TextureLoader.h"


class Engine
{
private:

	HRESULT hr_;
	std::chrono::steady_clock::time_point reference_;

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
	std::unique_ptr<GamePadInput> gamePadInput;

	std::unique_ptr<DebugCamera> debugCamera;
	std::unique_ptr<DepthStencil> depthStencil;

	std::unique_ptr<Draw> draw;
	std::unique_ptr<TextureLoader> textureLoader;

	GpuSyncManager gpuSyncManager;

	std::unique_ptr<ResourceBarrierHelper> resourceBarrierHelper;

	std::unique_ptr<GraphicsPipelineState> graphicsPipelineState;

	std::unique_ptr<DirectionalLight> directionalLight;
	std::unique_ptr<PointLight> pointLight;
	std::unique_ptr<SpotLight> spotLight;

	ID3D12DescriptorHeap* descriptorHeaps[1];

	static bool isEnd_;

public:

	~Engine();
	Engine(int32_t kClientWidth, int32_t kClientHeight);

	void Setting();

	void PostDraw();

	void NewFrame();

	void EndFrame();

	void End();

	void UpdateFixFPS();

	size_t GetProcessMemoryUsage();

	void Debug();

	Input* GetInput() { return input.get(); }

	static void SetEnd(bool isEnd) { isEnd_ = isEnd; }
	static bool IsEnd() { return isEnd_; }
};

