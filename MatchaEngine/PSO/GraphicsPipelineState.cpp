#include "GraphicsPipelineState.h"
#include "Core/LogHandler.h"
#include <cassert>
#include <d3d12shader.h>
#include <vector>

void GraphicsPipelineState::CreatePSO(const std::string& shaderName, BlendMode blendMode, std::ostream& os, ID3D12Device* device)
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

	std::string info = std::string("CreatePSO: shader=") + shaderName +
		", blend=" + BlendModeToString(blendMode) + "\n";
	Log(os, info);



	//1.	VS / PS を先にコンパイル
	directXShaderCompiler.CreateDXC();
	shaderCompile->CreateShaderCompile(shaderName, os, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());
	
	//2.	シェーダーリフレクションで b / t / s / u の使用状況を取得
	Microsoft::WRL::ComPtr<ID3D12ShaderReflection> vsReflection;
	Microsoft::WRL::ComPtr<IDxcBlob> vsBlob = shaderCompile->GetVertexShaderBlob();
	if (vsBlob) {
		DxcBuffer vsBuffer = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), 0 };
		directXShaderCompiler.GetDxcUtils()->CreateReflection(&vsBuffer, IID_PPV_ARGS(&vsReflection));
	}

	Microsoft::WRL::ComPtr<ID3D12ShaderReflection> psReflection;
	Microsoft::WRL::ComPtr<IDxcBlob> psBlob = shaderCompile->GetPixelShaderBlob();
	if (psBlob) {
		DxcBuffer psBuffer = { psBlob->GetBufferPointer(), psBlob->GetBufferSize(), 0 };
		directXShaderCompiler.GetDxcUtils()->CreateReflection(&psBuffer, IID_PPV_ARGS(&psReflection));
	}
	
	//3.	その情報から RootParameter / DescriptorRange を自動生成
	// Shaderで定義された各リソース（cbuffer, Texture2D等）に対し、自動でRootParameterを作成します
	std::vector<D3D12_ROOT_PARAMETER> rootParams;
	std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
	
	// 変数名からRootParameterのインデックスを逆引きするためのマップ
	std::map<std::string, UINT>& nameMap = rootParameterIndexMap_[shaderName][blendMode];
	nameMap.clear();

	// リフレクション結果を解析してRootParameterを構築するローカル関数
	auto ProcessReflection = [&](ID3D12ShaderReflection* reflection, D3D12_SHADER_VISIBILITY visibility) {
		if (!reflection) return;

		D3D12_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);

		for (UINT i = 0; i < shaderDesc.BoundResources; i++) {
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(i, &bindDesc);
			std::string name = bindDesc.Name;

			// 既に別のシェーダーステージ（例：VSとPSの両方）で同じ名前のリソースがバインドされていればスキップ
			if (nameMap.find(name) != nameMap.end()) continue;

			// CBV (Constant Buffer View) の場合: cbuffer
			if (bindDesc.Type == D3D_SIT_CBUFFER) {
				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				rootParam.ShaderVisibility = visibility;
				rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
				rootParam.Descriptor.RegisterSpace = bindDesc.Space;

				nameMap[name] = (UINT)rootParams.size();
				rootParams.push_back(rootParam);
			}
			// SRV (Shader Resource View) の場合: Texture2D, StructuredBuffer, ByteAddressBuffer など
			else if (bindDesc.Type == D3D_SIT_TEXTURE || bindDesc.Type == D3D_SIT_STRUCTURED || bindDesc.Type == D3D_SIT_BYTEADDRESS) {
				D3D12_DESCRIPTOR_RANGE range = {};
				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				range.BaseShaderRegister = bindDesc.BindPoint;
				range.NumDescriptors = bindDesc.BindCount;
				range.RegisterSpace = bindDesc.Space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				ranges.push_back(range);

				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParam.ShaderVisibility = visibility;

				nameMap[name] = (UINT)rootParams.size();
				rootParams.push_back(rootParam);
			}
			// UAV (Unordered Access View) の場合: RWTexture2D, RWStructuredBuffer など
			else if (bindDesc.Type == D3D_SIT_UAV_RWTYPED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED || bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS || bindDesc.Type == D3D_SIT_UAV_APPEND_STRUCTURED || bindDesc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED || bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER) {
				D3D12_DESCRIPTOR_RANGE range = {};
				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				range.BaseShaderRegister = bindDesc.BindPoint;
				range.NumDescriptors = bindDesc.BindCount;
				range.RegisterSpace = bindDesc.Space;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				ranges.push_back(range);

				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParam.ShaderVisibility = visibility;

				nameMap[name] = (UINT)rootParams.size();
				rootParams.push_back(rootParam);
			}
		}
	};

	// 頂点シェーダーとピクセルシェーダーの両方のリソースを解析する
	ProcessReflection(vsReflection.Get(), D3D12_SHADER_VISIBILITY_VERTEX);
	ProcessReflection(psReflection.Get(), D3D12_SHADER_VISIBILITY_PIXEL);

	// DescriptorTableを使用する場合、DescriptorRangeの配列へのポインタを設定する必要があるため
	// std::vectorが再確保されないこのタイミングでポインタを紐づけます。
	UINT rangeIdx = 0;
	for (auto& param : rootParams) {
		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			param.DescriptorTable.pDescriptorRanges = &ranges[rangeIdx++];
			param.DescriptorTable.NumDescriptorRanges = 1;
		}
	}

	rootSignature->GetDescriptionRootSignature().pParameters = rootParams.data();
	rootSignature->GetDescriptionRootSignature().NumParameters = (UINT)rootParams.size();

	sampler->CreateSampler(rootSignature->GetDescriptionRootSignature());
	
	//4.	RootSignature を生成
	rootSignature->CreateRootSignature(os, device);
	inputLayout->CreateInputLayout(shaderName);
	blendState->CreateBlendDesc(blendMode);
	rasterizerState->CreateRasterizerState(shaderName);
	depthStencilState->CreateDepthStencilState(shaderName);
	
	//5.	PSO を生成


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
	// 生成したいシェーダーのリスト（HLSLのベースファイル名）を定義します
	std::vector<std::string> shaderNames = {
		"Object3d",
		"SkinningObject3d",
		"Particle",
		"Line",
		"SkyBox"
	};

	// 各シェーダーに対して、全てのブレンドモードのPSOを生成してmapに保存します
	for (const auto& shader : shaderNames) {
		for (int j = 0; j < kBlendNum; j++) {
			CreatePSO(shader, BlendMode(j), os, device);
		}
	}
}
