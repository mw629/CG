#include "Engine.h"

#include <windows.h>
#include <psapi.h>
#include <iostream>

#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"
#include <thread>

#pragma comment(lib,"winmm.lib")

bool Engine::isEnd_ = false;

Engine::~Engine()
{

}

Engine::Engine(int32_t kClientWidth, int32_t kClientHeight)
{
	kClientWidth_ = kClientWidth;
	kClientHeight_ = kClientHeight;

	//時間の初期化
	reference_ = std::chrono::steady_clock::now();

	SetUnhandledExceptionFilter(ExportDump);
	logStream = CurrentTimestamp();

	//DebugLayer//
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif  

	graphics = std::make_unique<GraphicsDevice>(logStream);
	command = std::make_unique<CommandContext>(graphics->GetDevice());
	swapChain = std::make_unique<SwapChain>();
	descriptorHeap = std::make_unique<DescriptorHeap>(graphics->GetDevice());
	renderTargetView = std::make_unique<RenderTargetView>();
	viewportScissor = std::make_unique<ViewportScissor>(kClientWidth, kClientHeight);
	//入力
	input = std::make_unique<Input>();
	gamePadInput = std::make_unique<GamePadInput>();
	//描画
	debudCamera = std::make_unique<DebugCamera>();
	depthStencil = std::make_unique<DepthStencil>();
	draw = std::make_unique<Draw>();
	textureLoader = std::make_unique<TextureLoader>();
	//バリア
	resourceBarrierHelper = std::make_unique<ResourceBarrierHelper>();

	graphicsPipelineState = std::make_unique<GraphicsPipelineState>();

}

void Engine::Setting()
{

	window.DrawWindow(kClientWidth_, kClientHeight_);
	timeBeginPeriod(1);
	input->Initialize(window.GetWc(), window.GetHwnd());

	debudCamera->Initialize();

	//エラー・警告、即ちに停止//
#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(graphics->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//エラーと警告の抑制//
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/question/69805245/directx-12-application-is-crashing-in-windows--11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		//抑制レベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制
		infoQueue->PushStorageFilter(&filter);

		infoQueue->Release();
	}
#endif

	swapChain->CreateSwapChain(graphics->GetDxgiFactory(), command->GetCommandQueue(), window.GetHwnd(), kClientWidth_, kClientHeight_);
	descriptorHeap->CreateHeap(graphics->GetDevice());
	renderTargetView->CreateRenderTargetView(graphics->GetDevice(), swapChain->GetSwapChainResources(0), swapChain->GetSwapChainResources(1), descriptorHeap->GetRtvDescriptorHeap());

	gpuSyncManager.Initialize(graphics.get()->GetDevice());

	depthStencil->CreateDepthStencil(graphics->GetDevice(), kClientWidth_, kClientHeight_);


	graphicsPipelineState.get()->ALLPSOCreate(logStream, graphics.get()->GetDevice());

	directinalLight = std::make_unique<DirectinalLight>();
	directinalLight->CreateDirectinalLight(graphics->GetDevice());

	//ビューポート
	viewportScissor->CreateViewPort();
	//シーザー矩形
	viewportScissor->CreateSxissor();

	descriptorHeeps[0] = { descriptorHeap->GetSrvDescriptorHeap() };

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window.GetHwnd());
	ImGui_ImplDX12_Init(graphics->GetDevice(),
		swapChain->GetSwapChainDesc().BufferCount,
		renderTargetView->GetRtvDesc().Format,
		descriptorHeap->GetSrvDescriptorHeap(),
		descriptorHeap->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		descriptorHeap->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	Audio::Initialize();

	Draw::Initialize(command.get()->GetCommandList(), graphicsPipelineState.get(), directinalLight.get());
	Texture::Initalize(graphics->GetDevice(), command->GetCommandList(), descriptorHeap.get(), textureLoader.get());

	Line::SetDevice(graphics.get()->GetDevice());
	Grid::SetDevice(graphics.get()->GetDevice());
	Model::SetDevice(graphics.get()->GetDevice());
	Triangle::SetDevice(graphics.get()->GetDevice());
	Sprite::SetDevice(graphics.get()->GetDevice());
	Sphere::SetDevice(graphics.get()->GetDevice());

	Line::SetScreenSize({ (float)kClientWidth_,(float)kClientHeight_ });
	Grid::SetScreenSize({ (float)kClientWidth_,(float)kClientHeight_ });
	Model::SetScreenSize({ (float)kClientWidth_,(float)kClientHeight_ });
	Triangle::SetScreenSize({ (float)kClientWidth_,(float)kClientHeight_ });
	Sprite::SetScreenSize({ (float)kClientWidth_,(float)kClientHeight_ });
	Sphere::SetScreenSize({ (float)kClientWidth_,(float)kClientHeight_ });

}


void Engine::PostDraw()
{
	//ImGuiの描画コマンド
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command->GetCommandList());

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回RenderTargetからPresentにする
	resourceBarrierHelper->TransitionToPresent(command->GetCommandList());
}


void Engine::NewFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//コマンドを積み込んで確定させる//
	resourceBarrierHelper->Transition(command->GetCommandList(), swapChain.get());
	renderTargetView->SetAndClear(command->GetCommandList(), swapChain->GetSwapChain()->GetCurrentBackBufferIndex());
	//DSVを設定する
	depthStencil->SetDSV(command->GetCommandList(), renderTargetView->GetRtvHandles(swapChain->GetSwapChain()->GetCurrentBackBufferIndex()));
	//コマンドを積む//

	ID3D12DescriptorHeap* descriptorHeeps[] = { descriptorHeap->GetSrvDescriptorHeap() };
	command->GetCommandList()->SetDescriptorHeaps(1, descriptorHeeps);

	command->GetCommandList()->RSSetViewports(1, viewportScissor->GetViewport());//Viewportを設定
	command->GetCommandList()->RSSetScissorRects(1, viewportScissor->GetScissorRect());//Sxirssorを設定



	input.get()->Updata();
	gamePadInput.get()->Update();
}

void Engine::EndFrame() {

	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr_ = command->GetCommandList()->Close();
	assert(SUCCEEDED(hr_));



	//コマンドをキックする//

	//GPU2コマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { command->GetCommandList() };
	command->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
	
	//GPUとOSに画面の交換を行うよう通知する
	swapChain->GetSwapChain()->Present(1, 0);

	gpuSyncManager.Signal(command.get()->GetCommandQueue());

	gpuSyncManager.WaitForGpu();

	UpdateFixFPS();


	//次のフレーム用のコマンドを準備
	hr_ = command->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = command->GetCommandList()->Reset(command->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(hr_));
}

void Engine::End() {

	Audio::Finalize();

	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	window.Finalize();
}

void Engine::UpdateFixFPS()
{
	const std::chrono::microseconds kMinTIme(uint64_t(1000000.0f / 60.0f));
	const std::chrono::microseconds kMinCheckTIme(uint64_t(1000000.0f / 65.0f));

	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60秒立っていない場合
	if (elapsed < kMinTIme) {
		while (std::chrono::steady_clock::now() - reference_ < kMinTIme)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	reference_ = std::chrono::steady_clock::now();
}


size_t Engine::GetProcessMemoryUsage() {
	PROCESS_MEMORY_COUNTERS_EX pmc{};
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
		return pmc.WorkingSetSize; // 現在の物理メモリ使用量（バイト）
	}
	return 0;
}

void Engine::Debug()
{
#ifdef _DEBUG

	ImGui::Begin("Debug Info");

	//フレームレート (FPS)
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	// 1フレームあたりの時間 (ms)
	ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);

	//現在のフレーム数（自分でカウントする必要あり）
	static int frameCount = 0;
	frameCount++;
	ImGui::Text("Frame Count: %d", frameCount);

	size_t mem = GetProcessMemoryUsage();
	ImGui::Text("Memory Usage: %.2f MB", mem / (1024.0f * 1024.0f));

	textureLoader.get()->Draw();

	ImGui::End();

#endif
}