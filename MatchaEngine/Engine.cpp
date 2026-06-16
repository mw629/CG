#include "Engine.h"

#include <windows.h>
#include <psapi.h>
#include <iostream>

#ifdef _USE_IMGUI
#include "../externals/imgui/imgui.h"
#include "ImGuiManager.h"
#endif // _USE_IMGUI

#include <thread>

#pragma comment(lib,"winmm.lib")

bool Engine::isEnd_ = false;
bool Engine::isPlaying_ = false;
Engine::SceneOverlayCallback Engine::s_sceneOverlayCallback_ = nullptr;
Engine::EditorCallback Engine::s_saveCallback_ = nullptr;
Engine::EditorCallback Engine::s_loadCallback_ = nullptr;

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
	debugCamera = std::make_unique<DebugCamera>();
	depthStencil = std::make_unique<DepthStencil>();
	renderTexture = std::make_unique<RenderTexture>();
	draw = std::make_unique<Draw>();
	textureLoader = std::make_unique<TextureLoader>();
	//バリア
	resourceBarrierHelper = std::make_unique<ResourceBarrierHelper>();
	imGuiManager = std::make_unique<ImGuiManager>();

	graphicsPipelineState = std::make_unique<GraphicsPipelineState>();

}

void Engine::Setting()
{

	window.DrawWindow(kClientWidth_, kClientHeight_);
	timeBeginPeriod(1);
	input->Initialize(window.GetWc(), window.GetHwnd());

	debugCamera->Initialize();

	//エラー・警告、即ちに停止//
#ifdef _DEBUG
	Microsoft::WRL::ComPtr <ID3D12InfoQueue> infoQueue = nullptr;
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

	}
#endif

	swapChain->CreateSwapChain(graphics->GetDxgiFactory(), command->GetCommandQueue(), window.GetHwnd(), kClientWidth_, kClientHeight_);
	descriptorHeap->CreateHeap(graphics->GetDevice());
	renderTargetView->CreateRenderTargetView(graphics->GetDevice(), swapChain->GetSwapChainResources(), descriptorHeap->GetRtvDescriptorHeap());

	gpuSyncManager.Initialize(graphics.get()->GetDevice());

	depthStencil->CreateDepthStencil(graphics->GetDevice(), kClientWidth_, kClientHeight_);

	renderTexture->Initialize(graphics->GetDevice(), kClientWidth_, kClientHeight_, descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());

	graphicsPipelineState.get()->ALLPSOCreate(logStream, graphics.get()->GetDevice());


	lightManager = std::make_unique<LightManager>();
	lightManager->Initialize();

	//ビューポート
	viewportScissor->CreateViewPort();
	//シーザー矩形
	viewportScissor->CreateSxissor();

	descriptorHeaps[0] = { descriptorHeap->GetSrvDescriptorHeap() };


#ifdef _USE_IMGUI
	imGuiManager->Initialize(
		window.GetHwnd(), 
		graphics->GetDevice(), 
		swapChain->GetSwapChainDesc().BufferCount, 
		renderTargetView->GetRtvDesc().Format, 
		descriptorHeap->GetSrvDescriptorHeap(), 
		descriptorHeap->GetDescriptorSizeSRV(), 
		command->GetCommandQueue()
	);


	// 初回表示位置・サイズを厳密に指定（ImGuiCond_Once または ImGuiCond_Always に変更可能）
	ImGui::SetNextWindowPos(ImVec2(finalPos.x, finalPos.y), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(500.0f, 100.0f), ImGuiCond_Once);


#endif // _USE_IMGUI


	Audio::Initialize();

	Draw::Initialize(command.get()->GetCommandList(), graphicsPipelineState.get(),
		lightManager.get());
	Texture::Initialize(graphics->GetDevice(), command->GetCommandList(), descriptorHeap.get(), textureLoader.get());

	CharacterAnimator::SetData(graphics.get()->GetDevice(), descriptorHeap.get());
	//TransformAnimation::SetData(graphics.get()->GetDevice(), descriptorHeap.get());


	Vector2 Client = { (float)kClientWidth_,(float)kClientHeight_ };
	ObjectBase::SetObjectResource(Client);
	Line::SetScreenSize(Client);
	Grid::SetScreenSize(Client);
    EffectDefinition::SetScreenSize(Client);
	Triangle::SetScreenSize(Client);
	Sprite::SetScreenSize(Client);


  EffectDefinition::SetDescriptorHeap(descriptorHeap.get());
  EffectDefinition::SetDevice(graphics.get()->GetDevice());

  postEffect_ = std::make_unique<PostEffect>();
  postEffect_->Initialize();
}


void Engine::PostDraw()
{
	//Scene描画が終わったRenderTextureをShaderResourceへ
	renderTexture->TransitionToShaderResource(command->GetCommandList());

	//SwapchainをRenderTargetへ
	resourceBarrierHelper->Transition(command->GetCommandList(), swapChain.get());
	//RenderTargetの設定とClear（DepthStencilは設定・クリアしない）
	renderTargetView->SetAndClear(command->GetCommandList(), swapChain->GetSwapChain()->GetCurrentBackBufferIndex());

	command->GetCommandList()->RSSetViewports(1, viewportScissor->GetViewport());
	command->GetCommandList()->RSSetScissorRects(1, viewportScissor->GetScissorRect());

#ifdef _USE_IMGUI
	// _USE_IMGUI有効時はSceneウィンドウ内のImGui::Imageで描画するため
	// Swapchainへの全画面PostEffect描画はスキップ
#else
	Draw::DrawPostEffect(renderTexture->GetSrvHandleGPU(), PostEffect::GetActiveShaderName(), postEffect_.get());
#endif

#ifdef _USE_IMGUI
	imGuiManager->Render(command->GetCommandList());
#endif // _USE_IMGUI

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回RenderTargetからPresentにする
	resourceBarrierHelper->TransitionToPresent(command->GetCommandList());
}


void Engine::NewFrame() {

#ifdef _USE_IMGUI
	imGuiManager->NewFrame();
#endif // _USE_IMGUI

	//コマンドを積み込んで確定させる//
	renderTexture->TransitionToRenderTarget(command->GetCommandList());
	renderTexture->Clear(command->GetCommandList());

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture->GetRtvHandle();
	//DSVを設定する
	depthStencil->SetDSV(command->GetCommandList(), &rtvHandle);
	//コマンドを積む//

	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap->GetSrvDescriptorHeap() };
	command->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	command->GetCommandList()->RSSetViewports(1, viewportScissor->GetViewport());//Viewportを設定
	command->GetCommandList()->RSSetScissorRects(1, viewportScissor->GetScissorRect());//Scissorを設定



	input.get()->Updata();

	gamePadInput.get()->Update();

	if (postEffect_) {
		postEffect_->Update(1.0f / 60.0f);
	}
}

void Engine::EndFrame() {

	PostDraw();

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

#ifdef _USE_IMGUI
	imGuiManager->Shutdown();
#endif // _USE_IMGUI

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
	if (elapsed < kMinCheckTIme) {
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
		return pmc.WorkingSetSize; // 現在の物理メモリ使用量(バイト)
	}
	return 0;
}

void Engine::Debug()
{
#ifdef _USE_IMGUI

	// ===== Unity風メインメニューバー =====
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
				if (s_saveCallback_) s_saveCallback_();
			}
			if (ImGui::MenuItem("Load", "Ctrl+L")) {
				if (s_loadCallback_) s_loadCallback_();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				SetEnd(true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			ImGui::MenuItem("Debug Info", nullptr, &showFinalWindow);
			ImGui::EndMenu();
		}

		// --- メニューバー中央にPlay/Stopボタン ---
		float menuBarWidth = ImGui::GetWindowWidth();
		float buttonAreaWidth = 140.0f;
		ImGui::SetCursorPosX((menuBarWidth - buttonAreaWidth) * 0.5f);

		if (isPlaying_) {
			// Stopボタン（赤）
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.15f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.25f, 0.25f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
			if (ImGui::Button("  Stop  ")) {
				isPlaying_ = false;
			}
			ImGui::PopStyleColor(3);
		} else {
			// Playボタン（緑）
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.65f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.75f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.55f, 0.1f, 1.0f));
			if (ImGui::Button("  Play  ")) {
				isPlaying_ = true;
			}
			ImGui::PopStyleColor(3);
		}

		ImGui::EndMainMenuBar();
	}

	// Ctrl+S / Ctrl+L ショートカット
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
		if (s_saveCallback_) s_saveCallback_();
	}
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_L)) {
		if (s_loadCallback_) s_loadCallback_();
	}

	// ウィンドウ全体をDockSpaceとして設定（各ImGuiウィンドウをドッキング固定可能にする）
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::Begin("Debug Info");

	if (ImGui::BeginTabBar("DebugTabs")) {
		if (ImGui::BeginTabItem("System Data")) {
			ImGui::Text("--- Performance ---");
			ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

			static int frameCount = 0;
			frameCount++;
			ImGui::Text("Frame Count: %d", frameCount);

			size_t mem = GetProcessMemoryUsage();
			ImGui::Text("Memory Usage: %.2f MB", mem / (1024.0f * 1024.0f));
			ImGui::Spacing();

			ImGui::Text("--- Application ---");
			ImGui::Text("Resolution: %d x %d", kClientWidth_, kClientHeight_);
			ImGui::Spacing();

			ImGui::Text("--- Light Settings ---");
			static bool showLight = false;
			ImGui::Checkbox("Enable Light Settings (Shortcut: F1)", &showLight);
			if (ImGui::IsKeyPressed(ImGuiKey_F1))
			{
				showLight = !showLight;
			}
			
			if (showLight)
			{
				lightManager->ImGui();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Post Effect")) {
			postEffect_->ImGuiWindow();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Resource List")) {
			textureLoader.get()->Draw();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();

	ImGui::Begin("Scene");

	// --- アスペクト比設定 ---
	const char* aspectLabels[] = { "Free", "16:9", "4:3", "1:1", "21:9" };
	const float aspectRatios[] = { 0.0f, 16.0f / 9.0f, 4.0f / 3.0f, 1.0f, 21.0f / 9.0f };
	ImGui::Combo("Aspect Ratio", &sceneAspectRatioIndex_, aspectLabels, IM_ARRAYSIZE(aspectLabels));
	ImGui::Separator();

	ImVec2 availSize = ImGui::GetContentRegionAvail();
	ImVec2 imageSize = availSize;
	ImVec2 cursorStart = ImGui::GetCursorPos();

	if (sceneAspectRatioIndex_ > 0 && availSize.x > 0.0f && availSize.y > 0.0f) {
		float targetAspect = aspectRatios[sceneAspectRatioIndex_];
		float availAspect = availSize.x / availSize.y;

		if (availAspect > targetAspect) {
			// ウィンドウが横に広い → 高さに合わせ、左右に余白
			imageSize.y = availSize.y;
			imageSize.x = availSize.y * targetAspect;
		} else {
			// ウィンドウが縦に長い → 幅に合わせ、上下に余白
			imageSize.x = availSize.x;
			imageSize.y = availSize.x / targetAspect;
		}

		// 余白を計算して中央配置
		float offsetX = (availSize.x - imageSize.x) * 0.5f;
		float offsetY = (availSize.y - imageSize.y) * 0.5f;
		ImGui::SetCursorPos(ImVec2(cursorStart.x + offsetX, cursorStart.y + offsetY));
	}

	ImGui::Image((ImTextureID)renderTexture->GetSrvHandleGPU().ptr, imageSize);
	// ギズモ等のオーバーレイ描画コールバックをSceneウィンドウのBegin/Endの間に呼び出す
	if (s_sceneOverlayCallback_) {
		s_sceneOverlayCallback_();
	}
	ImGui::End();

#endif
}