#include "DirectXShaderCompiler.h"
#include "RootSignature.h"
#include "RootParameter.h"
#include "InputLayout.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "ShaderCompile.h"
#include "DepthStencilState.h"



class GraphicsPipelineState {
private:
	DirectXShaderCompiler* directXShaderCompiler;
	RootSignature* rootSignature_;
	RootParameter* rootParameter_;
	InputLayout* inputLayout_;
	BlendState* blendState_;
	RasterizerState* rasterizerState_;
	ShaderCompile* shaderCompile_;
	DepthStencilState* depthStencilState_;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};


	HRESULT hr_;

public:
	~GraphicsPipelineState();

	void PSOSetting(std::ostream& os, ID3D12Device* device);

	void CreatePSO(std::ostream& os, ID3D12Device* device);
};