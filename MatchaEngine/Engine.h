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
	std::ofstream logStream;
	GraphicsDevice* graphicsDevice;
public:

	~Engine();
	Engine();

	void Setting();

	void PreDraw();

	void PostDraw();


};

