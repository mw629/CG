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
		for (int i = 0; i < kNumLights; ++i)
		{
			std::string name = "Light " + std::to_string(i);
			if (::ImGui::TreeNode(name.c_str()))
			{
				::ImGui::Checkbox("Active", &active_[i]);
				::ImGui::ColorEdit3("Color", &color_[i].x);
				::ImGui::DragFloat3("Direction", &direction_[i].x, 0.01f, -1.0f, 1.0f, "%.2f");
				::ImGui::DragFloat("Intensity", &intensity_[i], 0.01f, 0.0f, 10.0f, "%.2f");
				::ImGui::TreePop();
			}

			// バッファに反映
			if (directionalLightData_)
			{
				directionalLightData_[i].color = color_[i];
				directionalLightData_[i].direction = direction_[i];
				directionalLightData_[i].intensity = intensity_[i];
				directionalLightData_[i].active = active_[i] ? 1 : 0;
			}
		}

		::ImGui::TreePop();
	}
#endif // _USE_IMGUI
}



void DirectionalLight::CreateDirectionalLight()
{
	directionalLightResource_ = GraphicsDevice::CreateBufferResource(sizeof(DirectionalLightData) * kNumLights);
	directionalLightData_ = nullptr;
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	for (int i = 0; i < kNumLights; ++i)
	{
		directionalLightData_[i].color = color_[i];
		directionalLightData_[i].direction = direction_[i];
		directionalLightData_[i].intensity = intensity_[i];
		directionalLightData_[i].active = active_[i] ? 1 : 0;
	}
}
