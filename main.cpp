#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <dxgidebug.h>
#include <dbgHelp.h>
#include <strsafe.h>
#include <dxcapi.h>
#include <vector>
#include <wrl.h> 
#include <xaudio2.h>

#include <memory> // For std::unique_ptr

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/d3dx12.h"

struct Debug {
	~Debug() {
		IDXGIDebug1* debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

#ifdef _DEBUG
std::unique_ptr<Debug> leakChacker = std::make_unique<Debug>();
#endif // _DEBUG


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


#include "MatchaEngine/Engine.h"


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dbgHelp.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"xaudio2.lib")






///クラス///





///-------------------------------------------
///WinMain
///------------------------------------------

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


	//std::unique_ptr<D3DResourceLeakChacker> leakChacker = std::make_unique<D3DResourceLeakChacker>(logStream);

	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);

	SetUnhandledExceptionFilter(ExportDump);

	std::ofstream logStream = CurrentTimestamp();

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

	//クライアント領域のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	//デバッグ用変数
	HRESULT hr;

	///クラス宣言///
	//設定
	WindowConfig window;
	std::unique_ptr<GraphicsDevice> graphics = std::make_unique<GraphicsDevice>(logStream);
	std::unique_ptr<CommandContext> command = std::make_unique<CommandContext>(graphics->GetDevice());
	std::unique_ptr<SwapChain> swapChain = std::make_unique<SwapChain>();
	std::unique_ptr<DescriptorHeap> descriptorHeap = std::make_unique<DescriptorHeap>(graphics->GetDevice());
	std::unique_ptr<RenderTargetView> renderTargetView = std::make_unique<RenderTargetView>();
	std::unique_ptr<ViewportScissor> viewportScissor = std::make_unique<ViewportScissor>(kClientWidth, kClientHeight);
	//入力
	std::unique_ptr<Input> input = std::make_unique<Input>();

	//描画
	std::unique_ptr<DebugCamera> debudCamera = std::make_unique<DebugCamera>();
	std::unique_ptr<DepthStencil> depthStencil = std::make_unique<DepthStencil>();

	std::unique_ptr<Draw> draw = std::make_unique<Draw>(command->GetCommandList());

	//フェンスやイベント、シグナル
	GpuSyncManager gpuSyncManager;

	//バリア
	std::unique_ptr<ResourceBarrierHelper> resourceBarrierHelper = std::make_unique<ResourceBarrierHelper>();

	//ウィンドウサイズの設定//
	window.DrawWindow(kClientWidth, kClientHeight);
	//キーの初期化
	input->Initialize(window.GeWc(), window.GetHwnd());
	//デバックカメラの初期化
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


	swapChain->CreateSwapChain(graphics->GetDxgiFactory(), command->GetCommandQueue(), window.GetHwnd(), kClientWidth, kClientHeight);
	descriptorHeap->CreateHeap(graphics->GetDevice());
	renderTargetView->CreateRenderTargetView(graphics->GetDevice(), swapChain->GetSwapChainResources(0), swapChain->GetSwapChainResources(1), descriptorHeap->GetRtvDescriptorHeap());


	//初期値0でFenceを作る
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	hr = graphics->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);


	depthStencil->CreateDepthStencil(graphics->GetDevice(), kClientWidth, kClientHeight);

	//----------------
	// PSO
	//----------------
	DirectXShaderCompiler directXShaderCompiler;
	std::unique_ptr<RootSignature> rootSignature = std::make_unique<RootSignature>();
	std::unique_ptr<RootParameter> rootParameter = std::make_unique<RootParameter>();
	std::unique_ptr<Sampler> sampler = std::make_unique<Sampler>();
	std::unique_ptr<InputLayout> inputLayout = std::make_unique<InputLayout>();
	std::unique_ptr<BlendState> blendState = std::make_unique<BlendState>();
	std::unique_ptr<RasterizerState> rasterizerState = std::make_unique<RasterizerState>();
	std::unique_ptr<ShaderCompile> shaderCompile = std::make_unique<ShaderCompile>();
	std::unique_ptr<DepthStencilState> depthStencilState = std::make_unique<DepthStencilState>();
	std::unique_ptr<GraphicsPipelineState> graphicsPipeState = std::make_unique<GraphicsPipelineState>();
	//DXCの初期化//
	directXShaderCompiler.CreateDXC();
	//DescriptorRange//
	//RootParameter//
	rootParameter->CreateRootParameter(rootSignature->GetDescriptionRootSignature());
	//Samplerの設定//
	sampler->CreateSampler(rootSignature->GetDescriptionRootSignature());
	//シリアライズしてバイナリする
	rootSignature->CreateRootSignature(logStream, graphics->GetDevice());
	//InputLayoutの設定を行う//
	inputLayout->CreateInputLayout();
	//BlendStateの設定を行う//
	blendState->CreateBlendDesc();
	//RasterizerStateの設定を行う//
	rasterizerState->CreateRasterizerState();
	//ShaderをCompileする//
	shaderCompile->CreateShaderCompile(logStream, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());
	//DepthStencilStateの設定//
	depthStencilState->CreateDepthStencilState();
	//PSO
	graphicsPipeState->PSOSetting(directXShaderCompiler, rootSignature.get(), rootParameter.get(),
		sampler.get(), inputLayout.get(), blendState.get(), rasterizerState.get(), shaderCompile.get(), depthStencilState.get());
	graphicsPipeState->CreatePSO(logStream, graphics->GetDevice());
	
	//----------------
	//Line PSO
	//----------------
	std::unique_ptr<RootSignature> linerootSignature = std::make_unique<RootSignature>();
	std::unique_ptr<RootParameter> linerootParameter = std::make_unique<RootParameter>();
	std::unique_ptr<InputLayout> lineinputLayout = std::make_unique<InputLayout>();
	std::unique_ptr<ShaderCompile> lineshaderCompile = std::make_unique<ShaderCompile>();
	std::unique_ptr<GraphicsPipelineState> lineGraphicsPipeState = std::make_unique<GraphicsPipelineState>(); //
	//RootParameter//
	linerootParameter->CreateLineRootParameter(linerootSignature->GetDescriptionRootSignature());
	//シリアライズしてバイナリする
	linerootSignature->CreateRootSignature(logStream, graphics->GetDevice());
	//InputLayoutの設定を行う//
	lineinputLayout->CreateLineInputLayout();
	//ShaderをCompileする//
	lineshaderCompile->CreateLineShaderCompile(logStream, directXShaderCompiler.GetDxcUtils(), directXShaderCompiler.GetDxcCompiler(), directXShaderCompiler.GetIncludeHandler());
	//PSO
	lineGraphicsPipeState->PSOSetting(directXShaderCompiler, linerootSignature.get(), linerootParameter.get(),
		sampler.get(), lineinputLayout.get(), blendState.get(), rasterizerState.get(), lineshaderCompile.get(), depthStencilState.get());
	lineGraphicsPipeState->CreateLinePSO(logStream, graphics->GetDevice());


	//Material用のResourceを作る//
	std::unique_ptr<MaterialFactory> material = std::make_unique<MaterialFactory>();
	std::unique_ptr<MaterialFactory> triangleMaterial = std::make_unique<MaterialFactory>();
	std::unique_ptr<MaterialFactory> spriteMaterial = std::make_unique<MaterialFactory>();
	std::unique_ptr<MaterialFactory> objMaterial = std::make_unique<MaterialFactory>();
	material->CreateMatrial(graphics->GetDevice(), false);
	triangleMaterial->CreateMatrial(graphics->GetDevice(), false);
	spriteMaterial->CreateMatrial(graphics->GetDevice(), false);
	objMaterial->CreateMatrial(graphics->GetDevice(), false);
	//テクスチャの作成
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	std::unique_ptr<TextureLoader> textureLoader = std::make_unique<TextureLoader>();
	texture->Initalize(graphics->GetDevice(), command->GetCommandList(), descriptorHeap.get(), textureLoader.get());
	texture->CreateTexture("resources/uvChecker.png");
	texture->CreateTexture("resources/nightSky.png");
	//線の描画
	std::unique_ptr<Line> line = std::make_unique<Line>();
	LineVertexData lineVertex[2] = { {{-1.0f,-1.0f,0.0f},{1.0f,1.0f,1.0f,1.0f} },
		{{ 1.0f,1.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }} };
	Transform lineTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	line.get()->CreateLine(graphics.get()->GetDevice());
	
	//三角形の作成
	std::unique_ptr<Triangle> triangle = std::make_unique<Triangle>();
	triangle->Initialize(material.get(), textureLoader->GetTexture(0));
	triangle->CreateTriangle(graphics->GetDevice());
	Transform triangleTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	Vector4 vertex[3] = {
	{ -0.1f, -0.1f, 0.0f, 1.0f },
	{ 0.0f, 0.1f, 0.0f, 1.0f },
	{ 0.1f, -0.1f, 0.0f, 1.0f }
	};
	//スプライト作成
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	sprite->Initialize(material.get(), textureLoader->GetTexture(0));
	sprite->CreateSprite(graphics->GetDevice());
	Transform spriteTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	//球の作成
	std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
	sphere->Initialize(material.get(), textureLoader->GetTexture(0));
	sphere->CreateSprite(graphics->GetDevice());
	Transform shpereTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	//モデルの作成
	ModelData modelData = LoadObjFile("resources/obj", "axis.obj");
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelData, material.get(), textureLoader->GetTexture(0));
	model->CreateModel(graphics->GetDevice());
	Transform objTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };



	std::unique_ptr<DirectinalLight> directinalLight = std::make_unique<DirectinalLight>();
	directinalLight->CreateDirectinalLight(graphics->GetDevice());



	//ViewportとScissor（シザー）//

	//ビューポート
	viewportScissor->CreateViewPort();
	//シーザー矩形
	viewportScissor->CreateSxissor();


	Matrix4x4 viewMatrix;

	bool debugCameraFlag = false;


	bool useMonsterBall = true;

	Transform camraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

	Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	int BGMHandle;
	audio->Initialize();
	BGMHandle = audio->Load("resources/Audio/BGM/sweet_pop.mp3");
	audio->Play(BGMHandle, false, 1.0f);


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

	//メインループ//

	MSG msg{};
	//ウィンドウのxが押されるまでループ
	while (msg.message != WM_QUIT) {
		//windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();


			//コマンドを積み込んで確定させる//

			resourceBarrierHelper->Transition(command->GetCommandList(), swapChain.get());


			//ゲーム処理



			//開発用UIの処理。実際に開発用UIを出す場合はここをゲ0無固有の処理に置き換える
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::Checkbox("debugCameraFlag", &debugCameraFlag);
			//ImGui::ShowDemoWindow(); debugCameraFlag

			textureLoader->Draw();

			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("camraTranslate", &camraTransform.translate.x, 0.01f);
				ImGui::DragFloat3("camraRotate", &camraTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("camraScale", &camraTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Triangle")) {
				ImGui::DragFloat3("TranslateTriangle", &triangleTransform.translate.x, 1.0f);
				ImGui::DragFloat3("RotateTriangle", &triangleTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("ScaleTriangle", &triangleTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("vertex")) {
				ImGui::DragFloat3("vertex 0", &vertex[0].x, 0.01f);
				ImGui::DragFloat3("vertex 1", &vertex[1].x, 0.01f);
				ImGui::DragFloat3("vertex 2", &vertex[2].x, 0.01f);
			}
			if (ImGui::CollapsingHeader("LVertex")) {
				ImGui::DragFloat3("Lvertex 0", &lineVertex[0].position.x, 0.01f);
				ImGui::DragFloat3("Lvertex 1", &lineVertex[1].position.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("SpriteUV"))
			{
				ImGui::DragFloat2("TranslateSpriteUV", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("ScaleSpriteUV", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("RotateSpriteUV", &uvTransformSprite.rotate.z);
			}



			input->Updata();

			if (debugCameraFlag) {
				debudCamera->Update(input.get());
				viewMatrix = debudCamera->GetViewMatrix();
			}
			else
			{
				Matrix4x4 cameraMatrix = MakeAffineMatrix(camraTransform.translate, camraTransform.scale, camraTransform.rotate);;
				viewMatrix = Inverse(cameraMatrix);
			}

			triangle->SetVertex(vertex);
			triangle->SetShape();
			triangle->SetTrandform(triangleTransform);
			triangle->SetWvp(viewMatrix);

			line.get()->SetVertex(lineVertex);
			line->SetTrandform(lineTransform);
			line.get()->SetWvp(viewMatrix);

			sprite->SetTrandform(spriteTransform);
			sprite->SetWvp();

			sphere->SetTrandform(shpereTransform);
			sphere->SetWvp(viewMatrix);

			model->SetTransform(objTransform);
			model->SetWvp(viewMatrix);





			Matrix4x4 uvTransformMatrix = Scale(uvTransformSprite.scale);
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Rotation(uvTransformSprite.rotate));
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Translation(uvTransformSprite.translate));
			//spriteMaterial->GetMaterialData()->uvTransform = uvTransformMatrix;
			//ImGuiの内部コマンドを生成
			ImGui::Render();


			renderTargetView->SetAndClear(command->GetCommandList(), swapChain->GetSwapChain()->GetCurrentBackBufferIndex());
			//DSVを設定する
			depthStencil->SetDSV(command->GetCommandList(), renderTargetView->GetRtvHandles(swapChain->GetSwapChain()->GetCurrentBackBufferIndex()));


			//コマンドを積む//

			ID3D12DescriptorHeap* descriptorHeeps[] = { descriptorHeap->GetSrvDescriptorHeap() };
			command->GetCommandList()->SetDescriptorHeaps(1, descriptorHeeps);
			command->GetCommandList()->SetPipelineState(graphicsPipeState->GetGraphicsPipelineState());//PSOを設定
			//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
			command->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			command->GetCommandList()->RSSetViewports(1, viewportScissor->GetViewport());//Viewportを設定
			command->GetCommandList()->RSSetScissorRects(1, viewportScissor->GetScissorRect());//Sxirssorを設定
			//RootSignatureを設定。POSに設定しているけど別途設定が必要
			command->GetCommandList()->SetGraphicsRootSignature(rootSignature->GetRootSignature());
			command->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLight->GetDirectinalLightResource()->GetGPUVirtualAddress());


			command->GetCommandList()->SetPipelineState(lineGraphicsPipeState->GetGraphicsPipelineState());//PSOを設定
			//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
			command->GetCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
			command->GetCommandList()->SetGraphicsRootSignature(linerootSignature->GetRootSignature());
			

			draw.get()->DrawLine(line.get());

			//三角形の描画
			//draw->DrawTriangle(triangle.get());
			//スプライトの描画	
			//draw.get()->DrawSprite(sprite.get());
			//球の描画
			//draw.get()->DrawShpere(sphere.get());
			//objectの描画
			//draw->DrawObj(model.get());




			//ImGuiの描画コマンド
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command->GetCommandList());


			//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
			//今回RenderTargetからPresentにする
			resourceBarrierHelper->TransitionToPresent(command->GetCommandList());
			//TransitionBarrierを張る


			//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
			hr = command->GetCommandList()->Close();
			assert(SUCCEEDED(hr));


			//コマンドをキックする//


			//GPU2コマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { command->GetCommandList() };
			command->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
			//GPUとOSに画面の交換を行うよう通知する
			swapChain->GetSwapChain()->Present(1, 0);



			fenceValue++;
			hr = command->GetCommandQueue()->Signal(fence, fenceValue);
			assert(SUCCEEDED(hr));

			if (fence->GetCompletedValue() < fenceValue) {
				hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
				assert(SUCCEEDED(hr));
				WaitForSingleObject(fenceEvent, INFINITE);
			}


			//次のフレーム用のコマンドを準備
			hr = command->GetCommandAllocator()->Reset();
			assert(SUCCEEDED(hr));
			hr = command->GetCommandList()->Reset(command->GetCommandAllocator(), nullptr);
			assert(SUCCEEDED(hr));
		}
	}



	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	//ウィンドウを閉じる
	CloseWindow(window.GetHwnd());


	if (fenceEvent) {
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}
	if (fence) {
		fence->Release();
		fence = nullptr;
	}
	
	depthStencil.reset();

	//COMの終了処理
	CoUninitialize();



	return 0;
}