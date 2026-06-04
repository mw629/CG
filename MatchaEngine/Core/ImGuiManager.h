#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>

class ImGuiManager {
public:
	ImGuiManager() = default;
	~ImGuiManager() = default;

	void Initialize(HWND hwnd, ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* srvDescriptorHeap, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);
	void NewFrame();
	void Render(ID3D12GraphicsCommandList* commandList);
	void Shutdown();

	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;
};
