#include "DirectXShaderCompiler.h"
#include "RootSignature.h"
#include "RootParameter.h"
#include "InputLayout.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "ShaderCompile.h"
#include "DepthStencilState.h"
#include "Sampler.h"



class GraphicsPipelineState {
private:
	DirectXShaderCompiler directXShaderCompiler_{};
	RootSignature* rootSignature_{};
	RootParameter* rootParameter_{};
	Sampler* sampler_{};
	InputLayout* inputLayout_{};
	BlendState* blendState_{};
	RasterizerState* rasterizerState_{};
	ShaderCompile* shaderCompile_{};
	DepthStencilState* depthStencilState_{};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	HRESULT hr_;

public:


	void PSOSetting(
		DirectXShaderCompiler directXShaderCompiler,
		RootSignature* rootSignature,
		RootParameter* rootParameter,
		Sampler* sampler,
		InputLayout* inputLayout,
		BlendState* blendState,
		RasterizerState* rasterizerState,
		ShaderCompile* shaderCompile,
		DepthStencilState* depthStencilState);

	void CreatePSO(std::ostream& os, ID3D12Device* device);

	void CreateLinePSO(std::ostream& os, ID3D12Device* device);


	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicsPipelineStateDesc()const { return graphicsPipelineStateDesc_; }
	ID3D12PipelineState* GetGraphicsPipelineState() { return graphicsPipelineState_.Get(); }
};