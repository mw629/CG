#include "DirectinalLight.h"
#include "Graphics/GraphicsDevice.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI

void DirectinalLight::ImGui()
{
#ifdef _USE_IMGUI
	if (::ImGui::TreeNode("DirectionalLight"))
	{
		::ImGui::ColorEdit3("Color", &color.x);
		::ImGui::DragFloat3("Direction", &direction.x, 0.01f, -1.0f, 1.0f);
		::ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 10.0f);

		// バッファに反映
		if (directinalLightData_)
		{
			directinalLightData_->color = color;
			directinalLightData_->direction = direction;
			directinalLightData_->intensity = intensity;
		}

		::ImGui::TreePop();
	}
#endif // _USE_IMGUI
}

void DirectinalLight::CreateDirectinalLight(ID3D12Device* device)
{
	directinalLightResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(DirectionalLight));
	directinalLightData_ = nullptr;
	directinalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData_));

	directinalLightData_->color = color;
	directinalLightData_->direction = direction;
	directinalLightData_->intensity = intensity;
}
