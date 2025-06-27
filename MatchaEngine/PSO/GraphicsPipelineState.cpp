#include "GraphicsPipelineState.h"

#include <cassert>



void GraphicsPipelineState::PSOSetting(std::ostream& os, ID3D12Device* device)
{
	//DXCの初期化//
	directXShaderCompiler.CreateDXC();

	//DescriptorRange//
	//RootParameter//
	rootParameter->CreateRootParameter(rootSignature.get()->GetDescriptionRootSignature());

	//Samplerの設定//
	sampler->CreateSampler(rootSignature.get()->GetDescriptionRootSignature());

	//シリアライズしてバイナリする
	rootSignature->CreateRootSignature(os, device);

	//InputLayoutの設定を行う//

	inputLayout->CreateInputLayout();

	//BlendStateの設定を行う//

	blendState->CreateBlendDesc();

	//RasterizerStateの設定を行う//

	rasterizerState->CreateRasterizerState();

	//ShaderをCompileする//

	shaderCompile->CreateShaderCompile(os, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());


	//DepthStencilStateの設定//

	depthStencilState->CreateDepthStencilState();
}

void GraphicsPipelineState::CreatePSO(std::ostream& os, ID3D12Device* device)
{
	PSOSetting(os,device);
	//PSOを生成する//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.get()->GetRootSignature();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayout.get()->GetInputLayoutDesc();//InputLayout
	graphicsPipelineStateDesc.VS = { shaderCompile.get()->GetVertexShaderBlob()->GetBufferPointer(),
	 shaderCompile.get()->GetVertexShaderBlob()->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { shaderCompile.get()->GetPixelShaderBlob()->GetBufferPointer(),
	shaderCompile.get()->GetPixelShaderBlob()->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc.BlendState = blendState.get()->GetBlendDesc();//BlenderState
	graphicsPipelineStateDesc.RasterizerState = rasterizerState.get()->GetRasterizerDesc();//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//作成したらPSOに代入、DSCのFormatを設定する//
	graphicsPipelineStateDesc.DepthStencilState = depthStencilState.get()->GetDepthStencilDesc();
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr_ = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr_));
}
