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
		::ImGui::ColorEdit3("Color", &color_.x);
		::ImGui::DragFloat3("Position", &position_.x, 0.01f, -100.0f, 100.0f);
		::ImGui::DragFloat("Intensity", &intensity_, 0.01f, 0.0f, 10.0f);
		::ImGui::DragFloat("Radius", &radius_, 0.01f, -100.0f, 100.0f);
		::ImGui::DragFloat("Decay", &decay_, 0.01f, -100.0f, 100.0f);

		// バッファに反映
		if (pointLightData_)
		{
			pointLightData_->color = color_;
			pointLightData_->position = position_;
			pointLightData_->intensity = intensity_;
			pointLightData_->radius = radius_;
			pointLightData_->decay = decay_;
		}

		::ImGui::TreePop();
	}
#endif // _USE_IMGUI
}

void PointLight::CreatePointLight()
{
	pointLightResource_ = GraphicsDevice::CreateBufferResource(sizeof(PointLightData));
	pointLightData_ = nullptr;
	pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));

	pointLightData_->color = color_;
	pointLightData_->position = position_;
	pointLightData_->intensity = intensity_;
	pointLightData_->radius = radius_;
	pointLightData_->decay = decay_;
}