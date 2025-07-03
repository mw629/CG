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


#include "Common/LogHandler.h"

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/d3dx12.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


#include "MatchaEngine/Resource/Audio.h"
#include "MatchaEngine/Resource/Load.h"
#include "MatchaEngine/Resource/Texture.h"
#include "MatchaEngine/Resource/TextureLoader.h"
#include "Common/GraphicsDevice.h"
#include "Common/CommandContext.h"
#include "Common/Input.h"
#include "Common/WindowConfig.h"
#include "GpuSyncManager.h"
#include "DescriptorHeap.h"
#include "DepthStencil.h"
#include "RenderTargetView.h"
#include "SwapChain.h"
#include "Math/Calculation.h" 
#include "GameObjects/DebugCamera.h"
#include "GameObjects/Draw.h"
#include "GameObjects/Matrial.h"
#include "ViewportScissor.h"
#include "DirectinalLight.h"
#include "ResourceBarrierHelper.h"
//PSO

#include "PSO/DirectXShaderCompiler.h"
#include "PSO/RootSignature.h"
#include "PSO/RootParameter.h"
#include "PSO/InputLayout.h"
#include "PSO/BlendState.h"
#include "PSO/RasterizerState.h"
#include "PSO/ShaderCompile.h"
#include "PSO/DepthStencilState.h"
#include <PSO/Sampler.h>
#include <PSO/GraphicsPipelineState.h>
#include <GameObjects/Sprite.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dbgHelp.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"xaudio2.lib")






///クラス///


//CrashHandlerの登録//

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
	//Dumpを出力する//

	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dump", nullptr);
	StringCchPrintf(filePath, MAX_PATH, L"./Dump/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	//preocessID(このexeのID)とクラッシュ(例外)の発生したthreadIDを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	_MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	//Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	//他に関連付けられているSEH例外ハンドルがあれば実行。通常のプロセスを終了する。

	return EXCEPTION_EXECUTE_HANDLER;
}


//CompileShader関数//
Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
	//ログ用
	std::ostream& os,
	//CompilerするShaderファイルのパス
	const std::wstring& filePath,
	//Compilerに使用するProfile
	const wchar_t* profile,
	//初期化で生成したもの3つ
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler)
{
	//hldlファイルを読み込む//

	//これからシェーダーをコンパイルする旨をログに出す
	Log(os, ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;//UTF8の文字コードであることを通知

	//Compilする//

	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProfileの設定
		L"-Zi",L"-Qembed_debug",//デバッグ用の情報埋め込み
		L"-Od",//最適化を外しておく
		L"-Zpr"//メモリレイアウトは最優先
	};
	//実際にShaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler.Get(),//includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	);
	//コンパイルエラーではなくdxcが起動できないなど致命的な結果
	assert(SUCCEEDED(hr));

	//警告・エラーが出ていないか確認する//

	//警告・エラーが出てきたらログを出して止める
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(os, shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
	}

	//Compile結果を受け取って返す//

	//コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	//成功したログを出す
	Log(os, ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを解放 (ComPtrが自動で解放するので不要)
	//shaderSource->Release();
	//shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}


Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeep(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesxriptors, bool shaderVisible)
{
	//ディスクリプタヒープの生成
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;//レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = numDesxriptors;//ダブルバッファ用に2つ。多くても別に構わない
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}


Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {

	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//Textureの幅
	resourceDesc.Height = height;//Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;//奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント、1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

	//深度地のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定、特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度地を書き込む状態にしておく
		&depthClearValue,//Clear最適地
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}




///-------------------------------------------
///WinMain
///------------------------------------------

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

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
	std::unique_ptr<CommandContext> command = std::make_unique<CommandContext>(graphics.get()->GetDevice());
	std::unique_ptr<SwapChain> swapChain = std::make_unique<SwapChain>();
	std::unique_ptr<DescriptorHeap> descriptorHeap = std::make_unique<DescriptorHeap>(graphics.get()->GetDevice());
	std::unique_ptr<RenderTargetView> renderTargetView = std::make_unique<RenderTargetView>();
	std::unique_ptr<ViewportScissor> viewportScissor = std::make_unique<ViewportScissor>(kClientWidth, kClientHeight);
	//入力
	Input* input = new Input;

	//描画
	DebugCamera* debudCamera = new DebugCamera;
	std::unique_ptr<DepthStencil> depthStencil = std::make_unique<DepthStencil>();
	std::unique_ptr<Matrial> matrial = std::make_unique<Matrial>();
	std::unique_ptr<Matrial> spriteMatrial = std::make_unique<Matrial>();
	std::unique_ptr<Matrial> objMatrial = std::make_unique<Matrial>();
	std::unique_ptr<Draw> draw = std::make_unique<Draw>(command.get()->GetCommandList());
	
	

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
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(graphics.get()->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
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


		//解放 (ComPtrが自動で解放するので不要)
		//infoQueue->Release();
	}
#endif


	swapChain.get()->CreateSwapChain(graphics.get()->GetDxgiFactory(), command.get()->GetCommandQueue(), window.GetHwnd(), kClientWidth, kClientHeight);
	descriptorHeap.get()->CreateHeap(graphics.get()->GetDevice());
	renderTargetView.get()->CreateRenderTargetView(graphics.get()->GetDevice(), swapChain.get()->GetSwapChainResources(0), swapChain.get()->GetSwapChainResources(1), descriptorHeap.get()->GetRtvDescriptorHeap());


	//初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	hr = graphics.get()->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);


	depthStencil->CreateDepthStencil(graphics.get()->GetDevice(), kClientWidth, kClientHeight);

	//Material用のResourceを作る//

	matrial->CreateMatrial(graphics->GetDevice(), false);
	spriteMatrial->CreateMatrial(graphics->GetDevice(), false);
	objMatrial->CreateMatrial(graphics->GetDevice(), false);
	//PSO

	DirectXShaderCompiler directXShaderCompiler;
	std::unique_ptr<RootSignature> rootSignature = std::make_unique<RootSignature>();
	std::unique_ptr<RootParameter> rootParameter = std::make_unique<RootParameter>();
	std::unique_ptr<Sampler> sampler = std::make_unique<Sampler>();
	std::unique_ptr<InputLayout> inputLayout = std::make_unique<InputLayout>();
	std::unique_ptr<BlendState> blendState = std::make_unique<BlendState>();
	std::unique_ptr<RasterizerState> rasterizerState = std::make_unique<RasterizerState>();
	std::unique_ptr<ShaderCompile> shaderCompile = std::make_unique<ShaderCompile>();
	std::unique_ptr<DepthStencilState> depthStencilState = std::make_unique<DepthStencilState>();
	std::unique_ptr<GraphicsPipelineState> graphicsPipelineState = std::make_unique<GraphicsPipelineState>();

	//DXCの初期化//
	directXShaderCompiler.CreateDXC();

	//DescriptorRange//
	//RootParameter//
	rootParameter->CreateRootParameter(rootSignature.get()->GetDescriptionRootSignature());

	//Samplerの設定//
	sampler->CreateSampler(rootSignature.get()->GetDescriptionRootSignature());

	//シリアライズしてバイナリする
	rootSignature->CreateRootSignature(logStream, graphics.get()->GetDevice());

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


	graphicsPipelineState->PSOSetting(directXShaderCompiler, rootSignature.get(), rootParameter.get(),
		sampler.get(), inputLayout.get(), blendState.get(), rasterizerState.get(), shaderCompile.get(), depthStencilState.get());
	graphicsPipelineState->CreatePSO(logStream, graphics.get()->GetDevice());




	//VertexResourceを生成する//
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(VertexData) * 6);

	//VertexxBuffViewを作成する//

	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);



	//Resouceにデータを書き込む//

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));


	//1枚目

	//左下
	vertexData[0].position = { -0.1f,-0.1f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData[1].position = { 0.0f,0.1f,0.0f,1.0f };
	vertexData[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[2].position = { 0.1f,-0.1f,0.0f,1.0f };
	vertexData[2].texcoord = { 1.0f,1.0f };

	//2枚
	//左下
	vertexData[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexData[3].texcoord = { 0.0f,1.0f };
	//上
	vertexData[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[4].texcoord = { 0.5f,0.0f };
	//右下
	vertexData[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexData[5].texcoord = { 1.0f,1.0f };

	//WVP用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(TransformationMatrix));
	//
	TransformationMatrix* wvpData = nullptr;
	//
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//
	wvpData->WVP = IdentityMatrix();
	wvpData->World = IdentityMatrix();


	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	std::unique_ptr<TextureLoader> textureLoader = std::make_unique<TextureLoader>();
	texture->Initalize(graphics.get()->GetDevice(), command.get()->GetCommandList(), descriptorHeap.get(), textureLoader.get());

	texture->CreateTexture("resources/uvChecker.png");

	texture->CreateTexture("resources/nightSky.png");


	//Sprote用の頂点リソースを作る//
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();
	sprite.get()->CreateSprite(graphics.get()->GetDevice());
	Transform spriteTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };

	ModelData modelData = LoadObjFile("resources/obj", "axis.obj");
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model.get()->Initialize(modelData, objMatrial.get(),textureLoader.get()->GetTexture(1));
	model.get()->CreateModel(graphics.get()->GetDevice());
	Transform objTransform = { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f } };
	

#pragma region 球円の描画
	//球円の描画//


	float pi = 3.14f;
	uint32_t kSubdivision = 16;

	//Shpere用の頂点リソースを作る//

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceShpere = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(VertexData) * (kSubdivision * kSubdivision) * 6);

	//頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewShpere{};
	//リソースの先頭アドレスから使う
	vertexBufferViewShpere.BufferLocation = vertexResourceShpere->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewShpere.SizeInBytes = sizeof(VertexData) * (kSubdivision * kSubdivision) * 6;
	//1頂点当たりのサイズ
	vertexBufferViewShpere.StrideInBytes = sizeof(VertexData);

	//頂点データを設定する//

	VertexData* vertexDataShpere = nullptr;
	vertexResourceShpere->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataShpere));

	//経度分割1つ分の角度φd
	const float kLonEvery = pi * 2.0f / float(kSubdivision);
	//緯度分割1つ分の角度Θd
	const float kLatEvery = pi / float(kSubdivision);
	//緯度の方向に分割	
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//Θ
		//経度方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;

			//一つ目の三角形

			VertexData a;
			VertexData b;
			VertexData c;
			VertexData d;

			//position

			a.position = {
				cos(lat) * cos(lon),
				sin(lat),
				cos(lat) * sin(lon),
				1.0f };

			b.position = {
				cos(lat + kLatEvery) * cos(lon),
				sin(lat + kLatEvery),
				cos(lat + kLatEvery) * sin(lon) ,
				1.0f };

			c.position = {
				cos(lat) * cos(lon + kLonEvery),
				sin(lat),
				cos(lat) * sin(lon + kLonEvery),
				1.0f };

			d.position = {
				cos(lat + kLatEvery) * cos(lon + kLonEvery),
				sin(lat + kLatEvery),
				cos(lat + kLatEvery) * sin(lon + kLonEvery),
				1.0f };

			//texcoord

			a.texcoord = {
				float(lonIndex) / float(kSubdivision),
				 float(latIndex) / float(kSubdivision) };

			b.texcoord = {
				float(lonIndex) / float(kSubdivision),
				 float(latIndex + 1) / float(kSubdivision) };

			c.texcoord = {
				float(lonIndex + 1) / float(kSubdivision),
				 float(latIndex) / float(kSubdivision) };

			d.texcoord = {
				float(lonIndex + 1) / float(kSubdivision),
				 float(latIndex + 1) / float(kSubdivision) };

			//法線ベクトルを計算する

			a.normal = Vector3(a.position.x, a.position.y, a.position.z);
			b.normal = Vector3(b.position.x, b.position.y, b.position.z);
			c.normal = Vector3(c.position.x, c.position.y, c.position.z);
			d.normal = Vector3(d.position.x, d.position.y, d.position.z);


			//頂点にデータを入力する。基準点a
			vertexDataShpere[startIndex] = a;
			vertexDataShpere[startIndex + 1] = c;
			vertexDataShpere[startIndex + 2] = b;
			//二つ目の三角形
			vertexDataShpere[startIndex + 3] = b;
			vertexDataShpere[startIndex + 4] = c;
			vertexDataShpere[startIndex + 5] = d;

		}
	}

	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceShpere = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataShpere = nullptr;
	//書き込むためのアドレスを取得
	transformationMatrixResourceShpere->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataShpere));
	//単位行列をかきこんでおく
	transformationMatrixDataShpere->WVP = IdentityMatrix();
	transformationMatrixDataShpere->World = IdentityMatrix();

#pragma endregion

	std::unique_ptr<DirectinalLight> directinalLight = std::make_unique<DirectinalLight>();
	directinalLight->CreateDirectinalLight(graphics.get()->GetDevice());

	

	//ViewportとScissor（シザー）//

	//ビューポート
	viewportScissor.get()->CreateViewPort();
	//シーザー矩形
	viewportScissor.get()->CreateSxissor();


	Matrix4x4 viewMatrix;

	bool debugCameraFlag = false;

	float rdius = 1.0f;
	bool useMonsterBall = true;

	Transform camraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };

	Transform transformShpere{ ScalarMultiply({1.0f,180.0f,1.0f},rdius),{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform transformObj{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,1.0f} };

	Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Audio* audio = new Audio;
	int BGMHandle;
	audio->Initialize();
	BGMHandle = audio->Load("resources/Audio/BGM/sweet_pop.mp3");
	audio->Play(BGMHandle, false, 1.0f);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window.GetHwnd());
	ImGui_ImplDX12_Init(graphics.get()->GetDevice(),
		swapChain.get()->GetSwapChainDesc().BufferCount,
		renderTargetView.get()->GetRtvDesc().Format,
		descriptorHeap.get()->GetSrvDescriptorHeap(),
		descriptorHeap.get()->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		descriptorHeap.get()->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

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

			resourceBarrierHelper->Transition(command.get()->GetCommandList(), swapChain.get());


			//ゲーム処理



			//開発用UIの処理。実際に開発用UIを出す場合はここをゲ0無固有の処理に置き換える
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			ImGui::Checkbox("debugCameraFlag", &debugCameraFlag);
			//ImGui::ShowDemoWindow();debugCameraFlag

			textureLoader.get()->Draw();

			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("camraTranslate", &camraTransform.translate.x, 0.01f);
				ImGui::DragFloat3("camraRotate", &camraTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("camraScale", &camraTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Sprite")) {
				ImGui::DragFloat3("TranslateSprite", &spriteTransform.translate.x, 0.01f);
				ImGui::DragFloat3("RotateSprite", &spriteTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("ScaleSprite", &spriteTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Shpere")) {
				ImGui::DragFloat3("TranslateShpere", &transformShpere.translate.x, 0.01f);
				ImGui::DragFloat3("RotateShpere", &transformShpere.rotate.x, 0.01f);
				ImGui::DragFloat3("ScaleShpere", &transformShpere.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Obj")) {
				ImGui::DragFloat3("TranslateObj", &transformObj.translate.x, 0.01f);
				ImGui::DragFloat3("RotateObj", &transformObj.rotate.x, 0.01f);
				ImGui::DragFloat3("ScaleObj", &transformObj.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("SpriteUV"))
			{
				ImGui::DragFloat2("TranslateSpriteUV", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("ScaleSpriteUV", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("RotateSpriteUV", &uvTransformSprite.rotate.z);
			}



			input->Updata();

			if (debugCameraFlag) {
				debudCamera->Update(input);
				viewMatrix = debudCamera->GetViewMatrix();
			}
			else
			{
				Matrix4x4 cameraMatrix = MakeAffineMatrix(camraTransform.translate, camraTransform.scale, camraTransform.rotate);;
				viewMatrix = Inverse(cameraMatrix);
			}
			//Matrix4x4 viewMatrix = Inverse(MakeAffineMatrix(camraTransform.translate, camraTransform.scale, camraTransform.rotate));
			Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);

			model.get()->SetTransform(objTransform);
			model.get()->SetWvp(viewMatrix);

			sprite.get()->SetTrandform(spriteTransform);
			sprite.get()->SetWvp();

			Matrix4x4 worldMatrixShpere = MakeAffineMatrix(transformShpere.translate, transformShpere.scale, transformShpere.rotate);
			Matrix4x4 worldViewProjectionMatrixShpere = MultiplyMatrix4x4(worldMatrixShpere, MultiplyMatrix4x4(viewMatrix, projectionMatri));
			*transformationMatrixDataShpere = { worldViewProjectionMatrixShpere,worldMatrixShpere };


			Matrix4x4 uvTransformMatrix = Scale(uvTransformSprite.scale);
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Rotation(uvTransformSprite.rotate));
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Translation(uvTransformSprite.translate));
			spriteMatrial.get()->GetMaterialData()->uvTransform = uvTransformMatrix;
			//ImGuiの内部コマンドを生成
			ImGui::Render();


			renderTargetView.get()->SetAndClear(command.get()->GetCommandList(), swapChain.get()->GetSwapChain()->GetCurrentBackBufferIndex());
			//DSVを設定する
			depthStencil->SetDSV(command.get()->GetCommandList(), renderTargetView.get()->GetRtvHandles(swapChain.get()->GetSwapChain()->GetCurrentBackBufferIndex()));




			//コマンドを積む//

			ID3D12DescriptorHeap* descriptorHeeps[] = { descriptorHeap.get()->GetSrvDescriptorHeap() };
			command.get()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeeps);

			command.get()->GetCommandList()->SetPipelineState(graphicsPipelineState.get()->GetGraphicsPipelineState());//PSOを設定
			command.get()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
			//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
			command.get()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//command.get()->GetCommandList()->IASetIndexBuffer(sprite->GetIndexBufferView());//IBVを設定


			command.get()->GetCommandList()->RSSetViewports(1, viewportScissor.get()->GetViewport());//Viewportを設定
			command.get()->GetCommandList()->RSSetScissorRects(1, viewportScissor.get()->GetScissorRect());//Sxirssorを設定
			//RootSignatureを設定。POSに設定しているけど別途設定が必要
			command.get()->GetCommandList()->SetGraphicsRootSignature(rootSignature.get()->GetRootSignature());
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLight.get()->GetDirectinalLightResource()->GetGPUVirtualAddress());

			//スプライトの描画	
			command.get()->GetCommandList()->IASetVertexBuffers(0, 1, sprite.get()->GetVertexBufferView());//VBVを設定
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteMatrial.get()->GetMaterialResource()->GetGPUVirtualAddress());
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, sprite.get()->GetVertexResource()->GetGPUVirtualAddress());
			command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureLoader.get()->GetTexture(1));

			//command.get()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
			command.get()->GetCommandList()->DrawInstanced(6, 1, 0, 0);

			////球の描画
			//command.get()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewShpere);//VBVを設定
			//command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteMatrial.get()->GetMaterialResource()->GetGPUVirtualAddress());
			//command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceShpere->GetGPUVirtualAddress());
			//if (!useMonsterBall) {
			//	command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			//}
			//else {
			//	command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU2);
			//}
			////描画
			////commandList->DrawInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0);
			//	
			
			//objectの描画
			draw->DrawObj(model.get());

			//ImGuiの描画コマンド
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command.get()->GetCommandList());


			//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
			//今回RenderTargetからPresentにする
			resourceBarrierHelper->TransitionToPresent(command.get()->GetCommandList());
			//TransitionBarrierを張る



			//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
			hr = command.get()->GetCommandList()->Close();
			assert(SUCCEEDED(hr));


			//コマンドをキックする//


			//GPU2コマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { command.get()->GetCommandList() };
			command.get()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
			//GPUとOSに画面の交換を行うよう通知する
			swapChain.get()->GetSwapChain()->Present(1, 0);



			fenceValue++;
			HRESULT hr = command.get()->GetCommandQueue()->Signal(fence.Get(), fenceValue);
			assert(SUCCEEDED(hr));

			if (fence->GetCompletedValue() < fenceValue) {
				HRESULT hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
				assert(SUCCEEDED(hr));
				WaitForSingleObject(fenceEvent, INFINITE);
			}


			//次のフレーム用のコマンドを準備
			hr = command.get()->GetCommandAllocator()->Reset();
			assert(SUCCEEDED(hr));
			hr = command.get()->GetCommandList()->Reset(command.get()->GetCommandAllocator(), nullptr);
			assert(SUCCEEDED(hr));
		}
	}



	//ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	//ウィンドウを閉じる
	CloseWindow(window.GetHwnd());

	//COMの終了処理
	CoUninitialize();


	return 0;
}