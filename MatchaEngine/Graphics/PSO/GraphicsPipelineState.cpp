#include "GraphicsPipelineState.h"
#include "../../Core/LogHandler.h"
#include <cassert>



GraphicsPipelineState::GraphicsPipelineState()
{
	rootSignature_ = std::make_unique<RootSignature>();
	rootParameter_ = std::make_unique<RootParameter>();
	sampler_ = std::make_unique<Sampler>();
	inputLayout_ = std::make_unique<InputLayout>();
	blendState_ = std::make_unique<BlendState>();
	rasterizerState_ = std::make_unique<RasterizerState>();
	shaderCompile_ = std::make_unique<ShaderCompile>();
	depthStencilState_ = std::make_unique<DepthStencilState>();
}

void GraphicsPipelineState::PSOSetting(ShaderName shaderName, BlendMode blendmode,std::ostream& os, ID3D12Device* device)
{

	Log(os, "///PSOの生成///\n");
	//DXCの初期化//
	directXShaderCompiler_.CreateDXC();
	Log(os, "DXCの初期化完了\n");
	//RootParameter//
	rootParameter_->CreateRootParameter(rootSignature_->GetDescriptionRootSignature());
	Log(os, "RootParameterの生成完了\n");
	//Samplerの設定//
	sampler_->CreateSampler(rootSignature_->GetDescriptionRootSignature());
	Log(os, "Samplerの生成完了\n");
	//シリアライズしてバイナリする
	rootSignature_->CreateRootSignature(os, device);
	Log(os, "RootSignatureの生成完了\n");
	//InputLayoutの設定を行う//
	inputLayout_->CreateInputLayout(shaderName);
	Log(os, "InputLayoutの生成完了\n");
	//BlendStateの設定を行う//
	blendState_->CreateBlendDesc(blendmode);
	Log(os, "BlendStateの生成完了\n");
	//RasterizerStateの設定を行う//
	rasterizerState_->CreateRasterizerState();
	Log(os, "RasterizerStateの生成完了\n");
	//ShaderをCompileする//
	shaderCompile_->CreateShaderCompile(shaderName,os, directXShaderCompiler_.GetDxcUtils(), directXShaderCompiler_.GetDxcCompiler(), directXShaderCompiler_.GetIncludeHandler());
	Log(os, "ShaderCompileの生成完了\n");
	//DepthStencilStateの設定//
	depthStencilState_->CreateDepthStencilState();
	Log(os, "DepthStencilStateの生成完了\n");


}

void GraphicsPipelineState::CreatePSO(ShaderName shaderName, BlendMode blendmode,std::ostream& os, ID3D12Device* device)
{
	//PSOを生成する//
	PSOSetting(shaderName, blendmode,os, device);

	graphicsPipelineStateDesc_[shaderName][blendmode].pRootSignature = rootSignature_->GetRootSignature();//RootSignature
	graphicsPipelineStateDesc_[shaderName][blendmode].InputLayout = inputLayout_->GetInputLayoutDesc();//InputLayout
	graphicsPipelineStateDesc_[shaderName][blendmode].VS = { shaderCompile_->GetVertexShaderBlob()->GetBufferPointer(),
	 shaderCompile_->GetVertexShaderBlob()->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc_[shaderName][blendmode].PS = { shaderCompile_->GetPixelShaderBlob()->GetBufferPointer(),
	shaderCompile_->GetPixelShaderBlob()->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc_[shaderName][blendmode].BlendState = blendState_->GetBlendDesc();//BlenderState
	graphicsPipelineStateDesc_[shaderName][blendmode].RasterizerState = rasterizerState_->GetRasterizerDesc();//RasterizerState
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
	graphicsPipelineStateDesc_[shaderName][blendmode].DepthStencilState = depthStencilState_->GetDepthStencilDesc();
	graphicsPipelineStateDesc_[shaderName][blendmode].DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成

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
