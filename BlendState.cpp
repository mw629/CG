#include "BlendState.h"

void BlendState::CreateBlendDesc()
{
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}
