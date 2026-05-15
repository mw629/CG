#pragma once
#include "ShaderCompiler.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Sampler.h"
#include <map>
#include <string>



class GraphicsPipelineState {
private:

	std::map<ShaderName, std::map<BlendMode, D3D12_GRAPHICS_PIPELINE_STATE_DESC>> graphicsPipelineStateDesc_;
	std::map<ShaderName, std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>>> graphicsPipelineState_;
	std::map<ShaderName, std::map<BlendMode, std::unique_ptr<RootSignature>>> rootSignature_;

	std::map<ShaderName, std::map<BlendMode, std::map<std::string, UINT>>> rootParameterIndexMap_;

	HRESULT hr_;

public:

	void CreatePSO(ShaderName shaderName, BlendMode blendMode, std::ostream& os, ID3D12Device* device);

	void CreateGraphicsPSO(const ShaderName& shaderName, const PipelineConfig& config, BlendMode blendMode, std::ostream& os, ID3D12Device* device);

	void ALLPSOCreate(std::ostream& os, ID3D12Device* device);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicsPipelineStateDesc(const ShaderName& shaderName, BlendMode blendMode) { return graphicsPipelineStateDesc_[shaderName][blendMode]; }
	ID3D12PipelineState* GetGraphicsPipelineState(const ShaderName& shaderName, BlendMode blendMode) { return graphicsPipelineState_[shaderName][blendMode].Get(); }
	RootSignature* GetRootSignature(const ShaderName& shaderName, BlendMode blendMode) { return rootSignature_[shaderName][blendMode].get(); }

	UINT GetRootParameterIndex(ShaderName shaderName, BlendMode blendMode, const std::string& name) {
		auto itShader = rootParameterIndexMap_.find(shaderName);
		if (itShader != rootParameterIndexMap_.end()) {
			auto itBlend = itShader->second.find(blendMode);
			if (itBlend != itShader->second.end()) {
				auto itName = itBlend->second.find(name);
				if (itName != itBlend->second.end()) {
					return itName->second;
				}
			}
		}
		return static_cast<UINT>(-1);
	}
};

