#include "GraphicsPipelineState.h"

#include <cassert>



void GraphicsPipelineState::PSOSetting(DirectXShaderCompiler directXShaderCompiler, RootSignature* rootSignature, RootParameter* rootParameter, Sampler* sampler, InputLayout* inputLayout, BlendState* blendState, RasterizerState* rasterizerState, ShaderCompile* shaderCompile, DepthStencilState* depthStencilState)
{
	//DXCの初期化//
	directXShaderCompiler_ = directXShaderCompiler;

	//DescriptorRange//
	//RootParameter//
	rootParameter_ = rootParameter;

	//Samplerの設定//
	sampler_ = sampler;

	//シリアライズしてバイナリする
	rootSignature_ = rootSignature;

	//InputLayoutの設定を行う//
	inputLayout_ = inputLayout;

	//BlendStateの設定を行う//
	blendState_ = blendState;

	//RasterizerStateの設定を行う//
	rasterizerState_ = rasterizerState;

	//ShaderをCompileする//
	shaderCompile_ = shaderCompile;

	//DepthStencilStateの設定//
	depthStencilState_ = depthStencilState;

}

void GraphicsPipelineState::CreatePSO(std::ostream& os, ID3D12Device* device)
{
	//PSOを生成する//

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
}
