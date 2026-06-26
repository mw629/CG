#include "ComputePipeline.h"
#include "VariableTypes.h"
#include "ShaderCompiler.h"
#include <map>
#include <vector>
#include <string>
#include <cassert>
#include <d3d12shader.h>

void ComputePipeline::CreatePipeline(std::ostream& os, ID3D12Device* device)
{
	DirectXShaderCompiler directXShaderCompiler{};
	directXShaderCompiler.CreateDXC();

	std::unique_ptr<ShaderCompile> shaderCompile = std::make_unique<ShaderCompile>();
	shaderCompile->CreateComputeShaderCompile(L"Resources/Shader/SkinningShader/SkinningObject3d.CS.hlsl", os, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());

	std::vector<D3D12_ROOT_PARAMETER> rootParams;
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
	ranges.reserve(32);
	std::map<std::string, UINT> nameMap;

	IDxcBlob* shaderBlob = shaderCompile->GetComputeShaderBlob();
	if (shaderBlob) {
		DxcBuffer reflectionData;
		reflectionData.Ptr = shaderBlob->GetBufferPointer();
		reflectionData.Size = shaderBlob->GetBufferSize();
		reflectionData.Encoding = DXC_CP_ACP;

		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> reflection;
		directXShaderCompiler.GetDxcUtils()->CreateReflection(&reflectionData, IID_PPV_ARGS(&reflection));

		D3D12_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);

		for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(i, &bindDesc);

			if (nameMap.find(bindDesc.Name) != nameMap.end()) {
				continue;
			}

			if (bindDesc.Type == D3D_SIT_CBUFFER) {
				D3D12_ROOT_PARAMETER param = {};
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
				param.Descriptor.ShaderRegister = bindDesc.BindPoint;
				param.Descriptor.RegisterSpace = bindDesc.Space;

				nameMap[bindDesc.Name] = static_cast<UINT>(rootParams.size());
				rootParams.push_back(param);
			}
			else if (bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_UAV_RWTYPED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED || bindDesc.Type == D3D_SIT_STRUCTURED) {
				D3D12_DESCRIPTOR_RANGE range = {};
				range.RangeType = (bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_STRUCTURED) ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV : D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				range.BaseShaderRegister = bindDesc.BindPoint;
				range.NumDescriptors = bindDesc.BindCount;
				range.RegisterSpace = bindDesc.Space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				ranges.push_back(range);

				D3D12_ROOT_PARAMETER param = {};
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &ranges.back();

				nameMap[bindDesc.Name] = static_cast<UINT>(rootParams.size());
				rootParams.push_back(param);
			}
		}
	}

	rootSignature_ = std::make_unique<RootSignature>();
	D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc = rootSignature_->GetDescriptionRootSignature();
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE; // Clear ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT for Compute
	rootSignatureDesc.pParameters = rootParams.data();
	rootSignatureDesc.NumParameters = static_cast<UINT>(rootParams.size());

	rootSignature_->CreateRootSignature(os, device);

	computePipelineStateDesc_.CS = {
		.pShaderBytecode = shaderBlob->GetBufferPointer(),
		.BytecodeLength = shaderBlob->GetBufferSize()
	};
	computePipelineStateDesc_.pRootSignature = rootSignature_->GetRootSignature();
	computePipelineStateDesc_.NodeMask = 0;
	computePipelineStateDesc_.CachedPSO.pCachedBlob = nullptr;
	computePipelineStateDesc_.CachedPSO.CachedBlobSizeInBytes = 0;
	computePipelineStateDesc_.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	
	HRESULT hr = device->CreateComputePipelineState(&computePipelineStateDesc_, IID_PPV_ARGS(&computePipelineState_));
	assert(SUCCEEDED(hr));

	uavDesc_.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc_.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc_.Buffer.FirstElement = 0;
	uavDesc_.Buffer.NumElements = 0; 
	uavDesc_.Buffer.CounterOffsetInBytes = 0;
	uavDesc_.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc_.Buffer.StructureByteStride = sizeof(VertexData);

	//device->CreateUnorderedAccessView(resouce, nullptr, &uavDesc_, descriptor);
}
