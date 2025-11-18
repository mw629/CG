#include "GraphicsPipelineState.h"
#include "../../Core/LogHandler.h"
#include <cassert>



void GraphicsPipelineState::CreatePSO(ShaderName shaderName, BlendMode blendmode, std::ostream& os, ID3D12Device* device)
{
	DirectXShaderCompiler directXShaderCompiler{};
	std::unique_ptr<RootSignature> rootSignature = std::make_unique<RootSignature>();
	std::unique_ptr<RootParameter> rootParameter = std::make_unique<RootParameter>();
	std::unique_ptr<Sampler> sampler = std::make_unique<Sampler>();
	std::unique_ptr<InputLayout> inputLayout = std::make_unique<InputLayout>();
	std::unique_ptr<BlendState> blendState = std::make_unique<BlendState>();
	std::unique_ptr<RasterizerState> rasterizerState = std::make_unique<RasterizerState>();
	std::unique_ptr<ShaderCompile>  shaderCompile = std::make_unique<ShaderCompile>();
	std::unique_ptr<DepthStencilState> depthStencilState = std::make_unique<DepthStencilState>();


	Log(os, "///PSOの生成///\n");
	//DXCの初期化//
	directXShaderCompiler.CreateDXC();
	Log(os, "DXCの初期化完了\n");
	//RootParameter//
	rootParameter->CreateRootParameter(rootSignature->GetDescriptionRootSignature(),shaderName);
	Log(os, "RootParameterの生成完了\n");
	//Samplerの設定//
	sampler->CreateSampler(rootSignature->GetDescriptionRootSignature());
	Log(os, "Samplerの生成完了\n");
	//シリアライズしてバイナリする
	rootSignature->CreateRootSignature(os, device);
	Log(os, "RootSignatureの生成完了\n");
	//InputLayoutの設定を行う//
	inputLayout->CreateInputLayout(shaderName);
	Log(os, "InputLayoutの生成完了\n");
	//BlendStateの設定を行う//
	blendState->CreateBlendDesc(blendmode);
	Log(os, "BlendStateの生成完了\n");
	//RasterizerStateの設定を行う//
	rasterizerState->CreateRasterizerState();
	Log(os, "RasterizerStateの生成完了\n");
	//ShaderをCompileする//
	shaderCompile->CreateShaderCompile(shaderName, os, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());
	Log(os, "ShaderCompileの生成完了\n");
	//DepthStencilStateの設定//
	depthStencilState->CreateDepthStencilState();
	Log(os, "DepthStencilStateの生成完了\n");



	graphicsPipelineStateDesc_[shaderName][blendmode].pRootSignature = rootSignature->GetRootSignature();//RootSignature
	graphicsPipelineStateDesc_[shaderName][blendmode].InputLayout = inputLayout->GetInputLayoutDesc();//InputLayout
	graphicsPipelineStateDesc_[shaderName][blendmode].VS = { shaderCompile->GetVertexShaderBlob()->GetBufferPointer(),
	 shaderCompile->GetVertexShaderBlob()->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc_[shaderName][blendmode].PS = { shaderCompile->GetPixelShaderBlob()->GetBufferPointer(),
	shaderCompile->GetPixelShaderBlob()->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc_[shaderName][blendmode].BlendState = blendState->GetBlendDesc();//BlenderState
	graphicsPipelineStateDesc_[shaderName][blendmode].RasterizerState = rasterizerState->GetRasterizerDesc();//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc_[shaderName][blendmode].NumRenderTargets = 1;
	graphicsPipelineStateDesc_[shaderName][blendmode].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc_[shaderName][blendmode].PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定(気にしなくていい)
	graphicsPipelineStateDesc_[shaderName][blendmode].SampleDesc.Count = 1;
	graphicsPipelineStateDesc_[shaderName][blendmode].SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//作成したらPSOに代入、DSCのFormatを設定する//
	graphicsPipelineStateDesc_[shaderName][blendmode].DepthStencilState = depthStencilState->GetDepthStencilDesc();
	graphicsPipelineStateDesc_[shaderName][blendmode].DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	rootSignature_[shaderName][blendmode] = std::move(rootSignature);

	hr_ = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_[shaderName][blendmode],
		IID_PPV_ARGS(&graphicsPipelineState_[shaderName][blendmode]));
	assert(SUCCEEDED(hr_));

	Log(os, "///PSOの生成///\n");
}

void GraphicsPipelineState::ALLPSOCreate(std::ostream& os, ID3D12Device* device)
{
	for (int i = 0; i < ShaderNum; i++) {
		for (int j = 0; j < kBlendNum; j++) {
			CreatePSO(ShaderName(i), BlendMode(j), os, device);
		}
	}
}
