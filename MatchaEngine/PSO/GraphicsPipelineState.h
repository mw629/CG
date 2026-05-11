#pragma once
#include "DirectXShaderCompiler.h"
#include "RootSignature.h"
#include "InputLayout.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "ShaderCompile.h"
#include "DepthStencilState.h"
#include "Sampler.h"
#include <map>
#include <string>



class GraphicsPipelineState {
private:
	// 以前の ShaderName 列挙型インデックスによる配列管理から、
	// HLSLファイル名（文字列）をキーとした map 管理へ変更しました。
	std::map<std::string, std::map<BlendMode, D3D12_GRAPHICS_PIPELINE_STATE_DESC>> graphicsPipelineStateDesc_;
	std::map<std::string, std::map<BlendMode, Microsoft::WRL::ComPtr<ID3D12PipelineState>>> graphicsPipelineState_;
	std::map<std::string, std::map<BlendMode, std::unique_ptr<RootSignature>>> rootSignature_;
	std::map<std::string, std::map<BlendMode, std::map<std::string, UINT>>> rootParameterIndexMap_;

	HRESULT hr_;

public:

	// シェーダーID（HLSLファイル名）を指定してPSOを生成します
	void CreatePSO(const std::string& shaderName, BlendMode blendMode, std::ostream& os, ID3D12Device* device);

	void CreateGraphicsPSO(const std::string& shaderName, BlendMode blendMode, std::ostream& os, ID3D12Device* device);

	// 定義された全てのシェーダー（文字列リストで管理）のPSOを生成します
	void ALLPSOCreate(std::ostream& os, ID3D12Device* device);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicsPipelineStateDesc(const std::string& shaderName, BlendMode blendMode) { return graphicsPipelineStateDesc_[shaderName][blendMode]; }
	ID3D12PipelineState* GetGraphicsPipelineState(const std::string& shaderName, BlendMode blendMode) { return graphicsPipelineState_[shaderName][blendMode].Get(); }
	RootSignature* GetRootSignature(const std::string& shaderName, BlendMode blendMode) { return rootSignature_[shaderName][blendMode].get(); }
	UINT GetRootParameterIndex(const std::string& shaderName, BlendMode blendMode, const std::string& name) {
		auto it = rootParameterIndexMap_[shaderName][blendMode].find(name);
		if (it != rootParameterIndexMap_[shaderName][blendMode].end()) {
			return it->second;
		}
		return static_cast<UINT>(-1);
	}
};

