#include "GraphicsPipelineState.h"
#include "LogHandler.h"
#include <cassert>



void GraphicsPipelineState::CreatePSO(ShaderName shaderName, BlendMode blendMode, std::ostream& os, ID3D12Device* device)
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

	std::string info = std::string("CreatePSO: shader=") + ShaderNameToString(shaderName) +
		", blend=" + BlendModeToString(blendMode) + "\n";
	Log(os, info);


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
	blendState->CreateBlendDesc(blendMode);
	Log(os, "BlendStateの生成完了\n");
	//RasterizerStateの設定を行う//
	rasterizerState->CreateRasterizerState(shaderName);
	Log(os, "RasterizerStateの生成完了\n");
	//ShaderをCompileする//
	shaderCompile->CreateShaderCompile(shaderName, os, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());
	Log(os, "ShaderCompileの生成完了\n");
	//DepthStencilStateの設定//
	depthStencilState->CreateDepthStencilState(shaderName);
	Log(os, "DepthStencilStateの生成完了\n");



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

void GraphicsPipelineState::CreateGraphicsPSO(ShaderName shaderName, BlendMode blendMode, std::ostream& os, ID3D12Device* device)
{
	//宣言
	DirectXShaderCompiler directXShaderCompiler{};
	std::unique_ptr<RootSignature> rootSignature = std::make_unique<RootSignature>();
	std::unique_ptr<RootParameter> rootParameter = std::make_unique<RootParameter>();
	std::unique_ptr<Sampler> sampler = std::make_unique<Sampler>();
	std::unique_ptr<InputLayout> inputLayout = std::make_unique<InputLayout>();
	std::unique_ptr<BlendState> blendState = std::make_unique<BlendState>();
	std::unique_ptr<RasterizerState> rasterizerState = std::make_unique<RasterizerState>();
	std::unique_ptr<ShaderCompile>  shaderCompile = std::make_unique<ShaderCompile>();
	std::unique_ptr<DepthStencilState> depthStencilState = std::make_unique<DepthStencilState>();

	std::string info = std::string("CreatePSO: shader=") + ShaderNameToString(shaderName) +
		", blend=" + BlendModeToString(blendMode) + "\n";
	Log(os, info);



	//1.	VS / PS を先にコンパイル
	
	//2.	シェーダーリフレクションで b / t / s / u の使用状況を取得
	
	//3.	その情報から RootParameter / DescriptorRange を自動生成
	
	//4.	RootSignature を生成
	
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
	for (int i = 0; i < ShaderNum; i++) {
		for (int j = 0; j < kBlendNum; j++) {
			CreatePSO(ShaderName(i), BlendMode(j), os, device);
		}
	}
}
