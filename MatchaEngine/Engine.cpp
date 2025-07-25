#include "Engine.h"

#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"

Engine::~Engine()
{

}

Engine::Engine(int32_t kClientWidth, int32_t kClientHeight)
{
	kClientWidth_ = kClientWidth;
	kClientHeight_ = kClientHeight;

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

	
	graphicsPipelineState.get()->CreateALLPSO(logStream, graphics.get()->GetDevice());

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

	Draw::Initialize(command.get()->GetCommandList());
	Texture::Initalize(graphics->GetDevice(), command->GetCommandList(), descriptorHeap.get(), textureLoader.get());
	MaterialFactory::SetDevice(graphics.get()->GetDevice());


	Line::SetDevice(graphics.get()->GetDevice());
	Grid::SetDevice(graphics.get()->GetDevice());
	Model::SetDevice(graphics.get()->GetDevice());
	Triangle::SetDevice(graphics.get()->GetDevice());
	Sprite::SetDevice(graphics.get()->GetDevice());
	Sphere::SetDevice(graphics.get()->GetDevice());

}

void Engine::PreDraw()
{
	command->GetCommandList()->SetPipelineState(graphicsPipelineState->GetGraphicsPipelineState());//PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	command->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//RootSignatureを設定。POSに設定しているけど別途設定が必要
	command->GetCommandList()->SetGraphicsRootSignature(graphicsPipelineState.get()->GetRootSignature()->GetRootSignature());
	command->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLight->GetDirectinalLightResource()->GetGPUVirtualAddress());
}

void Engine::PostDraw()
{
}

void Engine::LinePreDraw()
{
	command.get()->GetCommandList()->SetPipelineState(graphicsPipelineState->GetLineGraphicsPipelineState());//PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	command.get()->GetCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	//RootSignatureを設定。POSに設定しているけど別途設定が必要
	command.get()->GetCommandList()->SetGraphicsRootSignature(graphicsPipelineState.get()->GetLineRootSignature()->GetRootSignature());
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

}

void Engine::EndFrame() {
	//ImGuiの描画コマンド
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command->GetCommandList());


	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回RenderTargetからPresentにする
	resourceBarrierHelper->TransitionToPresent(command->GetCommandList());
	//TransitionBarrierを張る


	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr_ = command->GetCommandList()->Close();
	assert(SUCCEEDED(hr_));

	//コマンドをキックする//
	//GPU2コマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { command->GetCommandList() };
	command->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain->GetSwapChain()->Present(1, 0);

	//次のフレーム用のコマンドを準備
	hr_ = command->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = command->GetCommandList()->Reset(command->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(hr_));


}

void Engine::End() {
	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	//ウィンドウを閉じる
	CloseWindow(window.GetHwnd());

	//COMの終了処理
	CoUninitialize();
}

