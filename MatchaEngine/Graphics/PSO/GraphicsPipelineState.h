#pragma once
#include "DirectXShaderCompiler.h"
#include "RootSignature.h"
#include "RootParameter.h"
#include "InputLayout.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "ShaderCompile.h"
#include "DepthStencilState.h"
#include "Sampler.h"



class GraphicsPipelineState {
private:

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_[ShaderNum][kBlendNum] = {};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_[ShaderNum][kBlendNum] = { nullptr };
	std::unique_ptr<RootSignature> rootSignature_[ShaderNum][kBlendNum] = { nullptr };

	HRESULT hr_;

public:

	void CreatePSO(ShaderName shaderName, BlendMode blendmode, std::ostream& os, ID3D12Device* device);

	void ALLPSOCreate(std::ostream& os, ID3D12Device* device);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicsPipelineStateDesc(ShaderName shaderName, BlendMode blendmode)const { return graphicsPipelineStateDesc_[shaderName][blendmode]; }
	ID3D12PipelineState* GetGraphicsPipelineState(ShaderName shaderName, BlendMode blendmode) { return graphicsPipelineState_[shaderName][blendmode].Get(); }
	RootSignature* GetRootSignature(ShaderName shaderName, BlendMode blendmode) { return rootSignature_[shaderName][blendmode].get(); }
};

