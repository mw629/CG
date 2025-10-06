#include "BlendState.h"

D3D12_BLEND_DESC BlendState::CreateBlendDesc(BlendMode mode)
{
	blendDesc_.AlphaToCoverageEnable = FALSE;
	blendDesc_.IndependentBlendEnable = FALSE;

	auto& rt = blendDesc_.RenderTarget[0];
	rt.LogicOpEnable = FALSE;
	rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	switch (mode)
	{
	case BlendMode::kBlendModeNone:
		rt.BlendEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ZERO;
		rt.BlendOp = D3D12_BLEND_OP_ADD;
		break;

	case BlendMode::kBlendModeNormal:
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		rt.BlendOp = D3D12_BLEND_OP_ADD;
		break;

	case BlendMode::kBlendModeAdd:
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D12_BLEND_ONE;
		rt.BlendOp = D3D12_BLEND_OP_ADD;
		break;

	case BlendMode::kBlendModeSubtract:
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D12_BLEND_ONE;
		rt.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		break;

	case BlendMode::kBlendModeMultiply:
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D12_BLEND_DEST_COLOR;
		rt.DestBlend = D3D12_BLEND_ZERO;
		rt.BlendOp = D3D12_BLEND_OP_ADD;
		break;

	case BlendMode::kBlendModeScreen:
		rt.BlendEnable = TRUE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_INV_SRC_COLOR;
		rt.BlendOp = D3D12_BLEND_OP_ADD;
		break;

	default:
		break;
	}

	// アルファブレンドの設定
	/*rt.SrcBlendAlpha = D3D12_BLEND_ONE;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	rt.DestBlendAlpha = D3D12_BLEND_ZERO;*/
	rt.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	rt.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;

	return blendDesc_;

}




