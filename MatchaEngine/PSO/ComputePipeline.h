#pragma once
#include <d3d12.h>
#include <ostream>
#include <wrl/client.h>
class ComputePipeline
{
private:


	D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc_ = {};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState_ = nullptr;

public:

	void CreatePipeline(std::ostream& os, ID3D12Device* device);


};

