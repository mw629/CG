#pragma once
#include <fstream>
#include <chrono>
#include <functional>

///自作エンジン///

//Common
#include "CommandContext.h"

//Core
#include "LogHandler.h"
#include "VariableTypes.h"
#include "WindowConfig.h"
#include "ImGuiManager.h"

//GameObjects

//Light
#include "LightManager.h"

//Camera

#include "Camera.h"
#include "DebugCamera.h"


#include "MaterialFactory.h"
#include "Model.h"
#include "CharacterAnimator.h"
#include "TransformAnimation.h"
#include "EffectDefinition.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Sprite.h"
#include "Triangle.h"
#include "Ring.h"
#include "Cube.h"
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
#include "RenderTexture.h"
//PSO
#include "PipelineState.h"
#include "ShaderCompiler.h"
#include "GraphicsPipelineState.h"
#include "RootSignature.h"
#include "Sampler.h"

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


#include "Graphics/Render/PostEffect.h"

class Engine
{
public:
	// Sceneウィンドウ内のオーバーレイ描画用コールバック型
	using SceneOverlayCallback = std::function<void()>;
	// Save/Loadコールバック型
	using EditorCallback = std::function<void()>;

private:

	Vector2 finalPos = { 100.0f, 100.0f }; 
	bool showFinalWindow = true;

	// Sceneウィンドウのアスペクト比設定
	int sceneAspectRatioIndex_ = 2; // 0: Free, 1: 16:9, 2: 4:3, 3: 1:1, 4: 21:9

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
	std::unique_ptr<RenderTexture> renderTexture;

	std::unique_ptr<Draw> draw;
	std::unique_ptr<TextureLoader> textureLoader;
	std::unique_ptr<PostEffect> postEffect_;
	std::unique_ptr<ImGuiManager> imGuiManager;

	GpuSyncManager gpuSyncManager;

	std::unique_ptr<ResourceBarrierHelper> resourceBarrierHelper;

	std::unique_ptr<GraphicsPipelineState> graphicsPipelineState;
	std::unique_ptr<LightManager> lightManager;

	ID3D12DescriptorHeap* descriptorHeaps[1];

	static bool isEnd_;
	static bool isPlaying_;
	static SceneOverlayCallback s_sceneOverlayCallback_;
	static EditorCallback s_saveCallback_;
	static EditorCallback s_loadCallback_;

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
	PostEffect* GetPostEffect() { return postEffect_.get(); }

	static void SetEnd(bool isEnd) { isEnd_ = isEnd; }
	static bool IsEnd() { return isEnd_; }

	static void SetPlaying(bool playing) { isPlaying_ = playing; }
	static bool IsPlaying() { return isPlaying_; }

	static void SetSceneOverlayCallback(SceneOverlayCallback cb) { s_sceneOverlayCallback_ = cb; }
	static void ClearSceneOverlayCallback() { s_sceneOverlayCallback_ = nullptr; }

	static void SetSaveCallback(EditorCallback cb) { s_saveCallback_ = cb; }
	static void SetLoadCallback(EditorCallback cb) { s_loadCallback_ = cb; }
};


