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
	std::unique_ptr<RootSignature> rootSignature_{};
	std::unique_ptr<RootParameter> rootParameter_{};
	std::unique_ptr<Sampler> sampler_{};
	std::unique_ptr<InputLayout> inputLayout_{};
	std::unique_ptr<BlendState> blendState_{};
	std::unique_ptr<RasterizerState> rasterizerState_{};
	std::unique_ptr<ShaderCompile>  shaderCompile_{};
	std::unique_ptr<DepthStencilState> depthStencilState_{};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;

	std::unique_ptr<RootSignature> lineRootSignature_{};
	std::unique_ptr<RootParameter> lineRootParameter_{};
	std::unique_ptr<InputLayout> lineInputLayout_{};
	std::unique_ptr<ShaderCompile> lineShaderCompile_;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC lineGraphicsPipelineStateDesc_{};
	Microsoft::WRL::ComPtr<ID3D12PipelineState> lineGraphicsPipelineState_ = nullptr;

	HRESULT hr_;

public:
	GraphicsPipelineState();

	void CreateALLPSO(std::ostream& os, ID3D12Device* device);

	void PSOSetting(std::ostream& os, ID3D12Device* device);
	void CreatePSO(std::ostream& os, ID3D12Device* device);

	void LinePSOSetting(std::ostream& os, ID3D12Device* device);
	void CreateLinePSO(std::ostream& os, ID3D12Device* device);


	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetGraphicsPipelineStateDesc()const { return graphicsPipelineStateDesc_; }
	ID3D12PipelineState* GetGraphicsPipelineState() { return graphicsPipelineState_.Get(); }
	RootSignature* GetRootSignature() { return rootSignature_.get(); }

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetLineGraphicsPipelineStateDesc()const { return lineGraphicsPipelineStateDesc_; }
	ID3D12PipelineState* GetLineGraphicsPipelineState() { return lineGraphicsPipelineState_.Get(); }
	RootSignature* GetLineRootSignature() { return lineRootSignature_.get(); }

};