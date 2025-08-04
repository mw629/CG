#include "DirectionalLightManager.h"
#include "Graphics/GraphicsDevice.h"

void DirectionalLightManager::CreateDirectinalLight(ID3D12Device *device)
{
	 directinalLightResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(DirectionalLight));
	 directinalLightData_ = nullptr;
	directinalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData_));

	directinalLightData_->color = color_;
	directinalLightData_->direction = direction_;
	directinalLightData_->intensity = intensity_;
}
