#include "ComputePipeline.h"
#include "VariableTypes.h"

void ComputePipeline::CreatePipeline(std::ostream& os, ID3D12Device* device)
{
	computePipelineStateDesc_.CS = {
		.pShaderBytecode = nullptr,
		.BytecodeLength = 0
	};
	computePipelineStateDesc_.pRootSignature = nullptr;
	computePipelineState_ = nullptr;
	HRESULT hr = device->CreateComputePipelineState(&computePipelineStateDesc_, IID_PPV_ARGS(&computePipelineState_));

	uavDesc_.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc_.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc_.Buffer.FirstElement = 0;
	uavDesc_.Buffer.NumElements = 0; 
	uavDesc_.Buffer.CounterOffsetInBytes = 0;
	uavDesc_.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc_.Buffer.StructureByteStride = sizeof(VertexData);

	//device->CreateUnorderedAccessView(resouce, nullptr, &uavDesc_, descriptor);
	
}
