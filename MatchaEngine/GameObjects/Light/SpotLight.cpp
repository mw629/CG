#include "SpotLight.h"
#include <GraphicsDevice.h>
#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI

void SpotLight::ImGui()
{
#ifdef _USE_IMGUI


	if (::ImGui::TreeNode("SpotLight"))
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
				::ImGui::DragFloat3("Direction", &direction_[i].x, 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::DragFloat("Distance", &distance_[i], 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::DragFloat("Decay", &decay_[i], 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::DragFloat("CosAngle", &cosAngle_[i], 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::DragFloat("CosFalloffStart", &cosFalloffStart_[i], 0.01f, -100.0f, 100.0f, "%.2f");
				::ImGui::TreePop();
			}

			// バッファに反映
			if (spotLightData_)
			{
				spotLightData_[i].color = color_[i];
				spotLightData_[i].position = position_[i];
				spotLightData_[i].intensity = intensity_[i];
				spotLightData_[i].direction = direction_[i];
				spotLightData_[i].distance = distance_[i];
				spotLightData_[i].decay = decay_[i];
				spotLightData_[i].cosAngle = cosAngle_[i];
				spotLightData_[i].cosFalloffStart = cosFalloffStart_[i];
				spotLightData_[i].active = active_[i] ? 1 : 0;
			}
		}

		::ImGui::TreePop();
	}
#endif // _USE_IMGUI
}

void SpotLight::CreatePointLight()
{
	spotLightResource_ = GraphicsDevice::CreateBufferResource( sizeof(SpotLightData) * kNumLights);
	spotLightData_ = nullptr;
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));

	for (int i = 0; i < kNumLights; ++i)
	{
		spotLightData_[i].color = color_[i];
		spotLightData_[i].position = position_[i];
		spotLightData_[i].intensity = intensity_[i];
		spotLightData_[i].direction = direction_[i];
		spotLightData_[i].distance = distance_[i];
		spotLightData_[i].decay = decay_[i];
		spotLightData_[i].cosAngle = cosAngle_[i];
		spotLightData_[i].cosFalloffStart = cosFalloffStart_[i];
		spotLightData_[i].active = active_[i] ? 1 : 0;
	}
}