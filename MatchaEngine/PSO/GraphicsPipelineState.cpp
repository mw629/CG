#include "GraphicsPipelineState.h"
#include "LogHandler.h"
#include <cassert>
#include <d3d12shader.h>
#include <vector>



void GraphicsPipelineState::CreatePSO(ShaderName shaderName, BlendMode blendMode, std::ostream& os, ID3D12Device* device)
{
	assert(SUCCEEDED(hr_));

	Log(os, "///PSOの生成///\n");
}

void GraphicsPipelineState::CreateGraphicsPSO(const ShaderName& shaderName, const PipelineConfig& config, BlendMode blendMode, std::ostream& os, ID3D12Device* device)
{
	//宣言
	DirectXShaderCompiler directXShaderCompiler{};
	std::unique_ptr<RootSignature> rootSignature = std::make_unique<RootSignature>();
	std::unique_ptr<Sampler> sampler = std::make_unique<Sampler>();
	std::unique_ptr<InputLayout> inputLayout = std::make_unique<InputLayout>();
	std::unique_ptr<BlendState> blendState = std::make_unique<BlendState>();
	std::unique_ptr<RasterizerState> rasterizerState = std::make_unique<RasterizerState>();
	std::unique_ptr<ShaderCompile>  shaderCompile = std::make_unique<ShaderCompile>();
	std::unique_ptr<DepthStencilState> depthStencilState = std::make_unique<DepthStencilState>();

	std::string info = std::string("CreateGraphicsPSO: shader=") + ShaderNameToString(shaderName) +
		", blend=" + BlendModeToString(blendMode) + "\n";
	Log(os, info);



	//1.	VS / PS を先にコンパイル
	directXShaderCompiler.CreateDXC();
	shaderCompile->CreateShaderCompile(config, os, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());
	
	//2.	シェーダーリフレクションで b / t / s / u の使用状況を取得
	//3.	その情報から RootParameter / DescriptorRange を自動生成
	std::vector<D3D12_ROOT_PARAMETER> rootParams;
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
	// 領域を確保しておき、ポインタの無効化を防ぐ
	ranges.reserve(32);
	std::map<std::string, UINT>& nameMap = rootParameterIndexMap_[shaderName][blendMode];

	auto processReflection = [&](IDxcBlob* shaderBlob, D3D12_SHADER_VISIBILITY visibility) {
		if (!shaderBlob) return;
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
			
			// すでに登録済みのリソースならスキップするかVisibilityをALLにする
			if (nameMap.find(bindDesc.Name) != nameMap.end()) {
				rootParams[nameMap[bindDesc.Name]].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
				continue;
			}

			if (bindDesc.Type == D3D_SIT_CBUFFER) {
				D3D12_ROOT_PARAMETER param = {};
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				param.ShaderVisibility = visibility;
				param.Descriptor.ShaderRegister = bindDesc.BindPoint;
				param.Descriptor.RegisterSpace = bindDesc.Space;
				
				nameMap[bindDesc.Name] = static_cast<UINT>(rootParams.size());
				rootParams.push_back(param);
			} else if (bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_UAV_RWTYPED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED || bindDesc.Type == D3D_SIT_STRUCTURED) {
				D3D12_DESCRIPTOR_RANGE range = {};
				range.RangeType = (bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_STRUCTURED) ? D3D12_DESCRIPTOR_RANGE_TYPE_SRV : D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				range.BaseShaderRegister = bindDesc.BindPoint;
				range.NumDescriptors = bindDesc.BindCount;
				range.RegisterSpace = bindDesc.Space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				ranges.push_back(range);

				D3D12_ROOT_PARAMETER param = {};
				param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				param.ShaderVisibility = visibility;
				param.DescriptorTable.NumDescriptorRanges = 1;
				param.DescriptorTable.pDescriptorRanges = &ranges.back();
				
				nameMap[bindDesc.Name] = static_cast<UINT>(rootParams.size());
				rootParams.push_back(param);
			}
		}
	};

	processReflection(shaderCompile->GetVertexShaderBlob(), D3D12_SHADER_VISIBILITY_VERTEX);
	processReflection(shaderCompile->GetPixelShaderBlob(), D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_ROOT_SIGNATURE_DESC& rootSignatureDesc = rootSignature->GetDescriptionRootSignature();
	rootSignatureDesc.pParameters = rootParams.data();
	rootSignatureDesc.NumParameters = static_cast<UINT>(rootParams.size());

	// Sampler設定
	sampler->CreateSampler(rootSignatureDesc);

	//4.	RootSignature を生成
	rootSignature->CreateRootSignature(os, device);

	//5.	PSO を生成
	inputLayout->CreateInputLayout(config);
	blendState->CreateBlendDesc(blendMode);
	rasterizerState->CreateRasterizerState(config);
	depthStencilState->CreateDepthStencilState(config);


	graphicsPipelineStateDesc_[shaderName][blendMode].pRootSignature = rootSignature->GetRootSignature();//RootSignature
	graphicsPipelineStateDesc_[shaderName][blendMode].InputLayout = inputLayout->GetInputLayoutDesc();//InputLayout
	graphicsPipelineStateDesc_[shaderName][blendMode].VS = { shaderCompile->GetVertexShaderBlob()->GetBufferPointer(),
	 shaderCompile->GetVertexShaderBlob()->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc_[shaderName][blendMode].PS = { shaderCompile->GetPixelShaderBlob()->GetBufferPointer(),
	shaderCompile->GetPixelShaderBlob()->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc_[shaderName][blendMode].BlendState = blendState->GetBlendDesc();//BlenderState
	graphicsPipelineStateDesc_[shaderName][blendMode].RasterizerState = rasterizerState->GetRasterizerDesc();//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc_[shaderName][blendMode].NumRenderTargets = 1;
	graphicsPipelineStateDesc_[shaderName][blendMode].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc_[shaderName][blendMode].PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定(気にしなくていい)
	graphicsPipelineStateDesc_[shaderName][blendMode].SampleDesc.Count = 1;
	graphicsPipelineStateDesc_[shaderName][blendMode].SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//作成したらPSOに代入、DSCのFormatを設定する//
	graphicsPipelineStateDesc_[shaderName][blendMode].DepthStencilState = depthStencilState->GetDepthStencilDesc();
	graphicsPipelineStateDesc_[shaderName][blendMode].DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	rootSignature_[shaderName][blendMode] = std::move(rootSignature);

	hr_ = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_[shaderName][blendMode],
		IID_PPV_ARGS(&graphicsPipelineState_[shaderName][blendMode]));
	assert(SUCCEEDED(hr_));

	Log(os, "///PSOの生成///\n");

}


void GraphicsPipelineState::ALLPSOCreate(std::ostream& os, ID3D12Device* device)
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> objInput = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	std::vector<D3D12_INPUT_ELEMENT_DESC> animInput = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHT",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "INDEX",    0, DXGI_FORMAT_R32G32B32A32_SINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	std::vector<D3D12_INPUT_ELEMENT_DESC> particleInput = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	std::vector<D3D12_INPUT_ELEMENT_DESC> lineInput = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	std::vector<std::pair<ShaderName, PipelineConfig>> configs = {
		{ ObjectShader, { L"resources/Shader/Object3D.VS.hlsl", L"resources/Shader/Object3D.PS.hlsl", objInput, true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID } },
		{ AnimationObj, { L"resources/Shader/SkinningObject3d.VS.hlsl", L"resources/Shader/SkinningObject3d.PS.hlsl", animInput, true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID } },
		{ ParticleShader, { L"resources/Shader/Particle.VS.hlsl", L"resources/Shader/Particle.PS.hlsl", particleInput, true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID } },
		{ LineShader, { L"resources/Shader/Line.VS.hlsl", L"resources/Shader/Line.PS.hlsl", lineInput, true, D3D12_DEPTH_WRITE_MASK_ALL, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID } },
		{ SkyBoxShader, { L"resources/Shader/SkyBox.VS.hlsl", L"resources/Shader/SkyBox.PS.hlsl", objInput, true, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_CULL_MODE_FRONT, D3D12_FILL_MODE_SOLID } },
	};

	for (const auto& pair : configs) {
		for (int j = 0; j < kBlendNum; j++) {
			CreateGraphicsPSO(pair.first, pair.second, static_cast<BlendMode>(j), os, device);
		}
	}
}
