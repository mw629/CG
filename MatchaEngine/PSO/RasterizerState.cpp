#include "RasterizerState.h"

void RasterizerState::CreateRasterizerState(ShaderName shader)
{
	switch (shader)
	{
	case SkyBoxShader:
		rasterizerDesc_.CullMode = D3D12_CULL_MODE_FRONT;
		rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
		break;
	default:
		//裏面（時計回り）を表示しない
		//rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
		//両面を表示する（カリングなし）
		rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
		//三角形の中を塗りつぶす
		rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
		break;
	}
}


