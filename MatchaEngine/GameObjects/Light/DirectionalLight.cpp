#include "DirectionalLight.h"
#include "GraphicsDevice.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI

void DirectionalLight::ImGui()
{
#ifdef _USE_IMGUI
	if (::ImGui::TreeNode("DirectionalLight"))
	{
		::ImGui::ColorEdit3("Color", &color.x);
		::ImGui::DragFloat3("Direction", &direction.x, 0.01f, -1.0f, 1.0f);
		::ImGui::DragFloat("Intensity", &intensity, 0.01f, 0.0f, 10.0f);

		// バッファに反映
		if (directionalLightData_)
		{
			directionalLightData_->color = color;
			directionalLightData_->direction = direction;
			directionalLightData_->intensity = intensity;
		}

		::ImGui::TreePop();
	}
#endif // _USE_IMGUI
}



void DirectionalLight::CreateDirectionalLight()
{
	directionalLightResource_ = GraphicsDevice::CreateBufferResource(sizeof(DirectionalLight));
	directionalLightData_ = nullptr;
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	directionalLightData_->color = color;
	directionalLightData_->direction = direction;
	directionalLightData_->intensity = intensity;
}
