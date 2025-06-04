#include "GraphicsPipelineState.h"

GraphicsPipelineState::~GraphicsPipelineState()
{
	delete rootSignature_;
	delete rootParameter_;
	delete inputLayout_;
	delete blendState_;
}

void GraphicsPipelineState::CreatePSO(std::ostream& os)
{
	//<RootSignature>//
	//ShaderとResourceの関連付けを示したobj
	rootSignature_->CreateRootSignature(os);
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

	//<ShaderCompile>//
	//Shaderをコンパイルする

	//<DepthStencilState>//
	//深度にかかわる設定
}
