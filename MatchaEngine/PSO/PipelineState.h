#pragma once
#include <d3d12.h>

enum BlendMode {
	kBlendModeNone,
	kBlendModeNormal,
	kBlendModeAdd,
	kBlendModeSubtract,
	kBlendModeMultiply,
	kBlendModeScreen,

	kBlendNum,
};


#include <string>
#include <vector>

// シェーダーの名前を定数で管理
using ShaderName = std::string;
const ShaderName ObjectShader = "ObjectShader";
const ShaderName AnimationObj = "AnimationObj";
const ShaderName ParticleShader = "ParticleShader";
const ShaderName LineShader = "LineShader";
const ShaderName SkyBoxShader = "SkyBoxShader";

static const char* ShaderNameToString(const ShaderName& name)
{
	return name.c_str();
}

struct PipelineConfig {
	std::wstring vsPath;
	std::wstring psPath;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;

	// Depth Stencil Default
	bool depthEnable = true;
	D3D12_DEPTH_WRITE_MASK depthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	D3D12_COMPARISON_FUNC depthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// Rasterizer Default
	D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_NONE;
	D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID;
};

static const char* BlendModeToString(BlendMode mode)
{
	switch (mode) {
	case kBlendModeNone: return "kBlendModeNone";
	case kBlendModeNormal: return "kBlendModeNormal";
	case kBlendModeAdd: return "kBlendModeAdd";
	case kBlendModeSubtract: return "kBlendModeSubtract";
	case kBlendModeMultiply: return "kBlendModeMultiply";
	case kBlendModeScreen: return "kBlendModeScreen";
	default: return "UnknownBlendMode";
	}
}

class BlendState
{
private:
	D3D12_BLEND_DESC blendDesc_{};

public:
	D3D12_BLEND_DESC CreateBlendDesc(BlendMode mode);
	D3D12_BLEND_DESC GetBlendDesc() { return blendDesc_; }
};

class DepthStencilState
{
private:
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};

public:
	void CreateDepthStencilState(const PipelineConfig& config);
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc() { return depthStencilDesc_; }
};

class InputLayout
{
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

public:
	void CreateInputLayout(const PipelineConfig& config);
	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() { return inputLayoutDesc_; }
};

class RasterizerState
{
private:
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

public:
	void CreateRasterizerState(const PipelineConfig& config);
	D3D12_RASTERIZER_DESC GetRasterizerDesc() const { return rasterizerDesc_; };
};
