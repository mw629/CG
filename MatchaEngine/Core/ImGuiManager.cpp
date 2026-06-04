#include "ImGuiManager.h"

#ifdef _USE_IMGUI
#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_dx12.h"
#include "../../externals/imgui/imgui_impl_win32.h"
#endif


void ImGuiManager::Initialize(HWND hwnd, ID3D12Device* device, int bufferCount, DXGI_FORMAT rtvFormat, ID3D12DescriptorHeap* srvDescriptorHeap, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
#ifdef _USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		bufferCount,
		rtvFormat,
		srvDescriptorHeap,
		cpuHandle,
		gpuHandle);
#endif
}

void ImGuiManager::NewFrame() {
#ifdef _USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImGuiManager::Render(ID3D12GraphicsCommandList* commandList) {
#ifdef _USE_IMGUI
	//ImGuiの内部コマンドを生成
	ImGui::Render();
	//ImGuiの描画コマンド
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}

void ImGuiManager::Shutdown() {
#ifdef _USE_IMGUI
	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}
