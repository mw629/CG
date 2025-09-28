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
	
    lineRootSignature_ = std::make_unique<RootSignature>();
	lineRootParameter_ = std::make_unique<RootParameter>();
	lineInputLayout_ = std::make_unique<InputLayout>();
	lineShaderCompile_ = std::make_unique<ShaderCompile>();
}

void GraphicsPipelineState::CreateALLPSO(std::ostream& os, ID3D12Device* device)
{
	CreatePSO(os, device);
	CreateLinePSO(os, device);
}

void GraphicsPipelineState::PSOSetting(std::ostream& os, ID3D12Device* device)
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
	inputLayout_->CreateInputLayout();
	Log(os, "InputLayoutの生成完了\n");
	//BlendStateの設定を行う//
	blendState_->CreateBlendDesc(kBlendModeNone);
	Log(os, "BlendStateの生成完了\n");
	//RasterizerStateの設定を行う//
	rasterizerState_->CreateRasterizerState();
	Log(os, "RasterizerStateの生成完了\n");
	//ShaderをCompileする//
	shaderCompile_->CreateShaderCompile(os, directXShaderCompiler_.GetDxcUtils(), directXShaderCompiler_.GetDxcCompiler(), directXShaderCompiler_.GetIncludeHandler());
	Log(os, "ShaderCompileの生成完了\n");
	//DepthStencilStateの設定//
	depthStencilState_->CreateDepthStencilState();
	Log(os, "DepthStencilStateの生成完了\n");


}

void GraphicsPipelineState::CreatePSO(std::ostream& os, ID3D12Device* device)
{
	//PSOを生成する//
	PSOSetting(os, device);

	graphicsPipelineStateDesc_.pRootSignature = rootSignature_->GetRootSignature();//RootSignature
	graphicsPipelineStateDesc_.InputLayout = inputLayout_->GetInputLayoutDesc();//InputLayout
	graphicsPipelineStateDesc_.VS = { shaderCompile_->GetVertexShaderBlob()->GetBufferPointer(),
	 shaderCompile_->GetVertexShaderBlob()->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc_.PS = { shaderCompile_->GetPixelShaderBlob()->GetBufferPointer(),
	shaderCompile_->GetPixelShaderBlob()->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc_.BlendState = blendState_->GetBlendDesc();//BlenderState
	graphicsPipelineStateDesc_.RasterizerState = rasterizerState_->GetRasterizerDesc();//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc_.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定(気にしなくていい)
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//作成したらPSOに代入、DSCのFormatを設定する//
	graphicsPipelineStateDesc_.DepthStencilState = depthStencilState_->GetDepthStencilDesc();
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成

	hr_ = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr_));

	Log(os, "///PSOの生成///\n");
}

void GraphicsPipelineState::LinePSOSetting(std::ostream& os, ID3D12Device* device)
{

	Log(os, "///Line用のPSOの生成///\n");
	//RootParameter//
	lineRootParameter_->CreateLineRootParameter(lineRootSignature_->GetDescriptionRootSignature());
	Log(os, "Line用のRootParameterの生成完了\n");
	//シリアライズしてバイナリする
	lineRootSignature_->CreateRootSignature(os, device);
	Log(os, "Line用のRootSignatureの生成完了\n");
	//InputLayoutの設定を行う//
	lineInputLayout_->CreateLineInputLayout();
	Log(os, "Line用のInputLayoutの生成完了\n");
	//ShaderをCompileする//
	lineShaderCompile_->CreateLineShaderCompile(os, directXShaderCompiler_.GetDxcUtils(), directXShaderCompiler_.GetDxcCompiler(), directXShaderCompiler_.GetIncludeHandler());
	Log(os, "Line用のShaderCompileの生成完了\n");

}

void GraphicsPipelineState::CreateLinePSO(std::ostream& os, ID3D12Device* device)
{
	//PSOを生成する//
	LinePSOSetting(os,device);

	lineGraphicsPipelineStateDesc_.pRootSignature = lineRootSignature_->GetRootSignature();//RootSignature
	lineGraphicsPipelineStateDesc_.InputLayout = lineInputLayout_->GetLineInputLayoutDesc();//InputLayout
	lineGraphicsPipelineStateDesc_.VS = { lineShaderCompile_->GetVertexShaderBlob()->GetBufferPointer(),
	 lineShaderCompile_->GetVertexShaderBlob()->GetBufferSize() };//VertexShader
	lineGraphicsPipelineStateDesc_.PS = { lineShaderCompile_->GetPixelShaderBlob()->GetBufferPointer(),
	lineShaderCompile_->GetPixelShaderBlob()->GetBufferSize() };//PixelShader
	lineGraphicsPipelineStateDesc_.BlendState = blendState_->GetBlendDesc();//BlenderState
	lineGraphicsPipelineStateDesc_.RasterizerState = rasterizerState_->GetRasterizerDesc();//RasterizerState
	//書き込むRTVの情報
	lineGraphicsPipelineStateDesc_.NumRenderTargets = 1;
	lineGraphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	lineGraphicsPipelineStateDesc_.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	//どのように画面に色を打ち込むのかの設定(気にしなくていい)
	lineGraphicsPipelineStateDesc_.SampleDesc.Count = 1;
	lineGraphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//作成したらPSOに代入、DSCのFormatを設定する//
	lineGraphicsPipelineStateDesc_.DepthStencilState = depthStencilState_->GetDepthStencilDesc();
	lineGraphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成

	hr_ = device->CreateGraphicsPipelineState(&lineGraphicsPipelineStateDesc_,
		IID_PPV_ARGS(&lineGraphicsPipelineState_));
	assert(SUCCEEDED(hr_));


	Log(os, "///Line用のPSOの生成///\n");
}
