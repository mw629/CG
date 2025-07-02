#include "DirectinalLight.h"
#include "MatchaEngine/Common/GraphicsDevice.h"

void DirectinalLight::CreateDirectinalLight(ID3D12Device *device)
{
	 directinalLightResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(DirectionalLight));
	 directinalLightData_ = nullptr;
	directinalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData_));

	directinalLightData_->color = color;
	directinalLightData_->direction = direction;
	directinalLightData_->intensity = intensity;
}
