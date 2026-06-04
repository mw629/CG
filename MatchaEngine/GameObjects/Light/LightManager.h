#pragma once
#include <vector>
#include <wrl.h>
#include <d3dx12.h>
#include "Core/VariableTypes.h"
#include <numbers>

class LightManager
{
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
    DirectionalLightGroupData* directionalLightData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_;
    PointLightGroupData* pointLightData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_;
    SpotLightGroupData* spotLightData_ = nullptr;

    struct DirLightParams {
        Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
        Vector3 direction = { 0.0f,-1.0f,0.0f };
        float intensity = 1.0f;
        bool active = true;
    };

    struct PointLightParams {
        Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
        Vector3 position = { 0.0f,2.0f,0.0f };
        float intensity = 1.0f;
        float radius = 10.0f;
        float decay = 2.0f;
        bool active = true;
    };

    struct SpotLightParams {
        Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
        Vector3 position = { 2.0f,1.25f,0.0f };
        float intensity = 7.0f;
        Vector3 direction = { -1.0f,-1.0f,0.0f };
        float distance = 4.0f;
        float decay = 2.0f;
        float cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
        float cosFalloffStart = std::cos(std::numbers::pi_v<float> / 3.0f) + 0.1f;
        bool active = true;
    };

    std::vector<DirLightParams> dirLights_;
    std::vector<PointLightParams> pointLights_;
    std::vector<SpotLightParams> spotLights_;

    int kMaxLights = 10;

public:
    LightManager();
    void Initialize();
    void ImGui();
    void TransferData();

    ID3D12Resource* GetDirectionalLightResource() { return directionalLightResource_.Get(); }
    ID3D12Resource* GetPointLightResource() { return pointLightResource_.Get(); }
    ID3D12Resource* GetSpotLightResource() { return spotLightResource_.Get(); }
};
