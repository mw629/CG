#include "GraphicsPipelineState.h"

#include <cassert>

GraphicsPipelineState::~GraphicsPipelineState()
{
	delete directXShaderCompiler;
	delete rootSignature_;
	delete rootParameter_;
	delete inputLayout_;
	delete blendState_;
	delete rasterizerState_;
	delete shaderCompile_;
}

void GraphicsPipelineState::PSOSetting(std::ostream& os, Microsoft::WRL::ComPtr <ID3D12Device> device)
{
	//DXCの初期化
	
	directXShaderCompiler->CreateDXC();


	//<RootSignature>//
	//ShaderとResourceの関連付けを示したobj
	rootSignature_->CreateRootSignature(os,device);
	//<RootParameter>//
	//Shaderがどこでデータ読み込みするかまとめたもの
	rootParameter_->CreateRootParameter(rootSignature_->GetDescriptionRootSignature());
	//<INputLayout>//
	//VertexShaderへ渡す頂点データの指定
	inputLayout_->CreateInputLayout();
	//<BlendState>//
	//PixelShaderからの出力の書き込み方の設定
	blendState_->CreateBlendDesc();
	//<RasterizerState>//
	//Rasterizerの設定
	rasterizerState_->CreateRasterizerState();
	//<ShaderCompile>//
	//Shaderをコンパイルする
	shaderCompile_->CreateShaderCompile(os, directXShaderCompiler->GetDxcUtils(),
		directXShaderCompiler->GetDxcCompiler(), directXShaderCompiler->GetIncludeHandler());
	//<DepthStencilState>//
	//深度にかかわる設定
	depthStencilState_->CreateDepthStencilState( );
}

void GraphicsPipelineState::CreatePSO(std::ostream& os,Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	PSOSetting(os,device);
	graphicsPipelineStateDesc_.pRootSignature = rootSignature_->GetRootSignature().Get();//RootSignature
	graphicsPipelineStateDesc_.InputLayout = inputLayout_->GetInputLayoutDesc();//InputLayout
	graphicsPipelineStateDesc_.VS = { shaderCompile_->GetVertexShaderBlob().Get()->GetBufferPointer(),
	shaderCompile_->GetVertexShaderBlob().Get()->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc_.VS = { shaderCompile_->GetPixelShaderBlob().Get()->GetBufferPointer(),
	shaderCompile_->GetPixelShaderBlob().Get()->GetBufferSize() };//PixelShader
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
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr_ = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr_));
}
