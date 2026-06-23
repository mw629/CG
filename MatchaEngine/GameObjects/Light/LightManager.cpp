#include "LightManager.h"
#include "Graphics/GraphicsDevice.h"
#include <imgui.h>
#include <string>
#include "Math/Calculation.h"

LightManager::LightManager()
{
}

void LightManager::Initialize()
{
    // DirectionalLightResource
    directionalLightResource_ = GraphicsDevice::CreateBufferResource(sizeof(DirectionalLightGroupData));
    directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

    // PointLightResource
    pointLightResource_ = GraphicsDevice::CreateBufferResource(sizeof(PointLightGroupData));
    pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));

    // SpotLightResource
    spotLightResource_ = GraphicsDevice::CreateBufferResource(sizeof(SpotLightGroupData));
    spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));

    // Add 1 default light for each type to start with
    dirLights_.push_back(DirLightParams());
    pointLights_.push_back(PointLightParams());
    spotLights_.push_back(SpotLightParams());

    TransferData();
}

void LightManager::ImGui()
{
#ifdef _USE_IMGUI
    ImGui::Begin("Light Settings");

    // Directional Lights
    if (ImGui::TreeNode("Directional Lights")) {
        if (ImGui::Button("Add Directional Light") && dirLights_.size() < kMaxLights) {
            dirLights_.push_back(DirLightParams());
        }
        for (size_t i = 0; i < dirLights_.size(); ++i) {
            std::string label = "Directional Light " + std::to_string(i);
            if (ImGui::TreeNode(label.c_str())) {
                ImGui::Checkbox("Active", &dirLights_[i].active);
                ImGui::ColorEdit4("Color", &dirLights_[i].color.x);
                ImGui::DragFloat3("Direction", &dirLights_[i].direction.x, 0.01f, -1.0f, 1.0f);
                ImGui::DragFloat("Intensity", &dirLights_[i].intensity, 0.01f, 0.0f, 10.0f);
                
                if (ImGui::Button("Remove") && !dirLights_.empty()) {
                    dirLights_.erase(dirLights_.begin() + i);
                    ImGui::TreePop();
                    break;
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    // Point Lights
    if (ImGui::TreeNode("Point Lights")) {
        if (ImGui::Button("Add Point Light") && pointLights_.size() < kMaxLights) {
            pointLights_.push_back(PointLightParams());
        }
        for (size_t i = 0; i < pointLights_.size(); ++i) {
            std::string label = "Point Light " + std::to_string(i);
            if (ImGui::TreeNode(label.c_str())) {
                ImGui::Checkbox("Active", &pointLights_[i].active);
                ImGui::ColorEdit4("Color", &pointLights_[i].color.x);
                ImGui::DragFloat3("Position", &pointLights_[i].position.x, 0.1f);
                ImGui::DragFloat("Intensity", &pointLights_[i].intensity, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Radius", &pointLights_[i].radius, 0.1f, 0.0f, 100.0f);
                ImGui::DragFloat("Decay", &pointLights_[i].decay, 0.1f, 0.0f, 10.0f);
                
                if (ImGui::Button("Remove") && !pointLights_.empty()) {
                    pointLights_.erase(pointLights_.begin() + i);
                    ImGui::TreePop();
                    break;
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    // Spot Lights
    if (ImGui::TreeNode("Spot Lights")) {
        if (ImGui::Button("Add Spot Light") && spotLights_.size() < kMaxLights) {
            spotLights_.push_back(SpotLightParams());
        }
        for (size_t i = 0; i < spotLights_.size(); ++i) {
            std::string label = "Spot Light " + std::to_string(i);
            if (ImGui::TreeNode(label.c_str())) {
                ImGui::Checkbox("Active", &spotLights_[i].active);
                ImGui::ColorEdit4("Color", &spotLights_[i].color.x);
                ImGui::DragFloat3("Position", &spotLights_[i].position.x, 0.1f);
                ImGui::DragFloat3("Direction", &spotLights_[i].direction.x, 0.01f, -1.0f, 1.0f);
                ImGui::DragFloat("Intensity", &spotLights_[i].intensity, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Distance", &spotLights_[i].distance, 0.1f, 0.0f, 100.0f);
                ImGui::DragFloat("Decay", &spotLights_[i].decay, 0.1f, 0.0f, 10.0f);
                ImGui::DragFloat("CosAngle", &spotLights_[i].cosAngle, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("CosFalloffStart", &spotLights_[i].cosFalloffStart, 0.01f, 0.0f, 1.0f);
                
                if (ImGui::Button("Remove") && !spotLights_.empty()) {
                    spotLights_.erase(spotLights_.begin() + i);
                    ImGui::TreePop();
                    break;
                }
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();

    TransferData();
#endif
}

void LightManager::TransferData()
{
    // Directional
    directionalLightData_->numLights = static_cast<int32_t>(dirLights_.size());
    for (size_t i = 0; i < dirLights_.size(); ++i) {
        directionalLightData_->lights[i].color = dirLights_[i].color;
        directionalLightData_->lights[i].direction = Normalize(dirLights_[i].direction);
        directionalLightData_->lights[i].intensity = dirLights_[i].intensity;
        directionalLightData_->lights[i].active = dirLights_[i].active ? 1 : 0;
    }

    // Point
    pointLightData_->numLights = static_cast<int32_t>(pointLights_.size());
    for (size_t i = 0; i < pointLights_.size(); ++i) {
        pointLightData_->lights[i].color = pointLights_[i].color;
        pointLightData_->lights[i].position = pointLights_[i].position;
        pointLightData_->lights[i].intensity = pointLights_[i].intensity;
        pointLightData_->lights[i].radius = pointLights_[i].radius;
        pointLightData_->lights[i].decay = pointLights_[i].decay;
        pointLightData_->lights[i].active = pointLights_[i].active ? 1 : 0;
    }

    // Spot
    spotLightData_->numLights = static_cast<int32_t>(spotLights_.size());
    for (size_t i = 0; i < spotLights_.size(); ++i) {
        spotLightData_->lights[i].color = spotLights_[i].color;
        spotLightData_->lights[i].position = spotLights_[i].position;
        spotLightData_->lights[i].intensity = spotLights_[i].intensity;
        spotLightData_->lights[i].direction = Normalize(spotLights_[i].direction);
        spotLightData_->lights[i].distance = spotLights_[i].distance;
        spotLightData_->lights[i].decay = spotLights_[i].decay;
        spotLightData_->lights[i].cosAngle = spotLights_[i].cosAngle;
        spotLightData_->lights[i].cosFalloffStart = spotLights_[i].cosFalloffStart;
        spotLightData_->lights[i].active = spotLights_[i].active ? 1 : 0;
    }
}
