#include "LightManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

namespace {
	std::vector<DirectionalLight*> directionalLights_;
	std::vector<PointLight*> pointLights_;
	std::vector<SpotLight*> spotLights_;
}

void LightManager::RegisterLight(DirectionalLight* light)
{
	if (light) {
		directionalLights_.push_back(light);
	}
}

void LightManager::RegisterLight(PointLight* light)
{
	if (light) {
		pointLights_.push_back(light);
	}
}

void LightManager::RegisterLight(SpotLight* light)
{
	if (light) {
		spotLights_.push_back(light);
	}
}

void LightManager::ClearLights()
{
	directionalLights_.clear();
	pointLights_.clear();
	spotLights_.clear();
}

void LightManager::UpdateLightData()
{
	// ライト数を更新
	lightData_.lightCount.pointLightCount = static_cast<int>(pointLights_.size());
	lightData_.lightCount.spotLightCount = static_cast<int>(spotLights_.size());

	// TODO: 各ライトのデータをGPUバッファに転送する処理を実装
	// directionalLightsResource_, pointLightsResource_, spotLightsResource_ への書き込み
}

void LightManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex)
{
	// TODO: 実際のDescriptorTableをセットする処理を実装
}
