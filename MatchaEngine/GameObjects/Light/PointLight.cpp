#include "PointLight.h"
#include <GraphicsDevice.h>
#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI

void PointLight::ImGui()
{
#ifdef _USE_IMGUI


	if (::ImGui::TreeNode("pointLight"))
	{
		for (int i = 0; i < kNumLights; ++i)
		{
			std::string name = "Light " + std::to_string(i);
			if (::ImGui::TreeNode(name.c_str()))
			{
				::ImGui::Checkbox("Active", &active_[i]);
				::ImGui::ColorEdit3("Color", &color_[i].x);
				::ImGui::DragFloat3("Position", &position_[i].x, 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::DragFloat("Intensity", &intensity_[i], 0.01f, 0.0f, 10.0f, "%.2f");
				::ImGui::DragFloat("Radius", &radius_[i], 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::DragFloat("Decay", &decay_[i], 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::TreePop();
			}

			// バッファに反映
			if (pointLightData_)
			{
				pointLightData_[i].color = color_[i];
				pointLightData_[i].position = position_[i];
				pointLightData_[i].intensity = intensity_[i];
				pointLightData_[i].radius = radius_[i];
				pointLightData_[i].decay = decay_[i];
				pointLightData_[i].active = active_[i] ? 1 : 0;
			}
		}

		::ImGui::TreePop();
	}
#endif // _USE_IMGUI
}

void PointLight::CreatePointLight()
{
	pointLightResource_ = GraphicsDevice::CreateBufferResource(sizeof(PointLightData) * kNumLights);
	pointLightData_ = nullptr;
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));

	for (int i = 0; i < kNumLights; ++i)
	{
		pointLightData_[i].color = color_[i];
		pointLightData_[i].position = position_[i];
		pointLightData_[i].intensity = intensity_[i];
		pointLightData_[i].radius = radius_[i];
		pointLightData_[i].decay = decay_[i];
		pointLightData_[i].active = active_[i] ? 1 : 0;
	}
}