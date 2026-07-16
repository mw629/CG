#pragma once
#include <d3d12.h>
#include <ostream>
#include <wrl/client.h>
#include <memory>
#include "RootSignature.h"
#include <map>
class ComputePipeline
{
private:


	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState_ = nullptr;

	std::unique_ptr<RootSignature> rootSignature_ = nullptr;
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc_ = {};

	std::map<std::string, UINT> rootParameterIndexMap_;

public:

	void CreatePipeline(std::ostream& os, ID3D12Device* device);
	ID3D12PipelineState* GetPipelineState() { return computePipelineState_.Get(); }
	ID3D12RootSignature* GetRootSignature() { return rootSignature_->GetRootSignature(); }

	UINT GetRootParameterIndex(const std::string& name) {
		auto it = rootParameterIndexMap_.find(name);
		if (it != rootParameterIndexMap_.end()) return it->second;
		return static_cast<UINT>(-1);
	}


};

