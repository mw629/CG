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
		::ImGui::ColorEdit3("Color", &color_.x);
		::ImGui::DragFloat3("Position", &position_.x, 0.01f, -100.0f, 100.0f);
		::ImGui::DragFloat("Intensity", &intensity_, 0.01f, 0.0f, 10.0f);
		::ImGui::DragFloat3("Direction", &direction_.x, 0.01f, -100.0f, 100.0f);
		::ImGui::DragFloat("Distance", &distance_, 0.01f, -100.0f, 100.0f);
		::ImGui::DragFloat("Decay", &decay_, 0.01f, -100.0f, 100.0f);
		::ImGui::DragFloat("CosAngle", &cosAngle_, 0.01f, -100.0f, 100.0f);
		::ImGui::DragFloat("CosFalloffStart", &cosFalloffStart_, 0.01f, -100.0f, 100.0f);

		// バッファに反映
		if (spotLightData_)
		{
			spotLightData_->color = color_;
			spotLightData_->position = position_;
			spotLightData_->intensity = intensity_;
			spotLightData_->direction = direction_;
			spotLightData_->distance = distance_;
			spotLightData_->decay = decay_;
			spotLightData_->cosAngle = cosAngle_;
			spotLightData_->cosFalloffStart = cosFalloffStart_;
		}

		::ImGui::TreePop();
	}
#endif // _USE_IMGUI
}

void SpotLight::CreatePointLight(ID3D12Device* device)
{
	spotLightResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(SpotLightData));
	spotLightData_ = nullptr;
	spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));

	spotLightData_->color = color_;
	spotLightData_->position = position_;
	spotLightData_->intensity = intensity_;
	spotLightData_->direction = direction_;
	spotLightData_->distance = distance_;
	spotLightData_->decay = decay_;
	spotLightData_->cosAngle = cosAngle_;
	spotLightData_->cosFalloffStart = cosFalloffStart_;
	
}