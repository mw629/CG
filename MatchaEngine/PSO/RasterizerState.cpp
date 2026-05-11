#include "RasterizerState.h"

void RasterizerState::CreateRasterizerState(const std::string& shader)
{
	if (shader == "SkyBox")
	{
		rasterizerDesc_.CullMode = D3D12_CULL_MODE_FRONT;
		rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	}
	else
	{
		//裏面（時計回り）を表示しない
		//rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
		//両面を表示する（カリングなし）
		rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
		//三角形の中を塗りつぶす
		rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	}
}


