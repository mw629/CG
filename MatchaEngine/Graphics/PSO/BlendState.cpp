#include "BlendState.h"

void BlendState::CreateBlendDesc()
{
	//すべての色要素を書き込む
	blendDesc_.AlphaToCoverageEnable = FALSE;
	blendDesc_.IndependentBlendEnable = FALSE; // 複数のレンダーターゲットがある場合はTRUEに設定
	blendDesc_.RenderTarget[0].BlendEnable = TRUE; // 最初のレンダーターゲットのブレンドを有効にする
	blendDesc_.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースブレンドファクター: ソースアルファ
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // デスティネーションブレンドファクター: 1 - ソースアルファ
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // ブレンド演算: 加算
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; // アルファブレンドファクター 
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // アルファブレンドファクター 
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // すべてのカラーチャンネルが書き込まれることを確認
}
