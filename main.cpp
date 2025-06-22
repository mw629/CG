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

#include "Math/Calculation.h"//MTで作った
#include "MatchaEngine/Resource/Audio.h"
#include "MatchaEngine/Common/GraphicsDevice.h"
#include "Common/WindowConfig.h"
#include "Object/Camera.h"
#include "Object/Draw.h"
#include "PSO/DirectXShaderCompiler.h"
#include "Common/Input.h"
#include "Object/DebugCamera.h"
#include "Common/CommandContext .h"
#include "SwapChain.h"
#include "DescriptorHeap.h"
#include "RenderTargetView.h"
#include "MatchaEngine/Object/Model.h"
#include <Resource/Load.h>

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

std::vector<std::vector<bool>> LoadBoolMapFromCSV(const std::string& filename) {
	std::vector<std::vector<bool>> map;
	std::ifstream file(filename);
	std::string line;

	if (!file.is_open()) {
		return map;
	}
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string cell;
		std::vector<bool> row;

		while (std::getline(ss, cell, ',')) {
			if (cell == "1") {
				row.push_back(true);
			}
			else {
				row.push_back(false);
			}
		}
		map.insert(map.begin(), row);
	}

	return map;
}


struct Coner
{
	Vector3 leftTop = { 0,0,0 };
	Vector3 rightTop = { 0,0,0 };
	Vector3 leftBottom = { 0,0,0 };
	Vector3 rightBottom = { 0,0,0 };
};

Coner GetConer(Vector3 pos, float size) {
	Coner coner{};
	coner.leftTop = Vector3{ pos.x - size / 2,pos.y + size / 2 ,0 };
	coner.rightTop = Vector3{ pos.x + size / 2,pos.y + size / 2 ,0 };
	coner.leftBottom = Vector3{ pos.x - size / 2,pos.y - size / 2,0 };
	coner.rightBottom = Vector3{ pos.x + size / 2,pos.y - size / 2,0 };
	return coner;
}

bool IsHitBlock(Vector3 pos, const std::vector<std::vector<bool>>& map) {
	int mapX = static_cast<int>((pos.x - 1.0f) / 2.0f);
	int mapY = static_cast<int>((pos.y - 1.0f) / 2.0f);

	if (mapY < 0 || mapY >= map.size() || mapX < 0 || mapX >= map[0].size()) {
		return false;
	}
	return map[mapY][mapX]; // true = ブロックあり
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
	//std::unique_ptr<RenderTargetView> renderTargetView = std::make_unique<RenderTargetView>();
	//入力
	Input* input = new Input;

	//描画
	DebugCamera* debudCamera = new DebugCamera;
	Camera camera;
	Draw draw;


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
	//renderTargetView.get()->CreateRenderTargetView(graphics.get()->GetDevice(), swapChain.get()->GetSwapChainResources(), descriptorHeap.get()->GetRtvDescriptorHeap());


	//RTVを作る//

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する(ここが変わるかも05-01)
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = descriptorHeap.get()->GetRtvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//まず一つ目作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles[0] = rtvStartHandle;
	graphics.get()->GetDevice()->CreateRenderTargetView(swapChain.get()->GetSwapChainResources(0), &rtvDesc, rtvHandles[0]);
	//2つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles[1].ptr = rtvHandles[0].ptr + graphics.get()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	graphics.get()->GetDevice()->CreateRenderTargetView(swapChain.get()->GetSwapChainResources(1), &rtvDesc, rtvHandles[1]);






	//初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	hr = graphics.get()->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);


	//Material用のResourceを作る//

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(Material));
	//マテリアルデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレス取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は書き込んでみる
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->endbleLighting = true;
	materialData->uvTransform = Identity();

	//Sprite用のマテリアルリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(Material));
	//マテリアルデータを書き込む
	Material* materialDataSprite = nullptr;
	//書き込むためのアドレス取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//今回は書き込んでみる
	materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialDataSprite->endbleLighting = false;
	materialDataSprite->uvTransform = Identity();

	//Sprite用のマテリアルリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> dethmaterialResourceSprite = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(Material));
	//マテリアルデータを書き込む
	Material* dethmaterialDataSprite = nullptr;
	//書き込むためのアドレス取得
	dethmaterialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&dethmaterialDataSprite));
	//今回は書き込んでみる
	dethmaterialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 0.1f);
	dethmaterialDataSprite->endbleLighting = false;
	dethmaterialDataSprite->uvTransform = Identity();

	//Sprite用のマテリアルリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> kuromaterialResourceSprite = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(Material));
	//マテリアルデータを書き込む
	Material* kuromaterialDataSprite = nullptr;
	//書き込むためのアドレス取得
	kuromaterialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&kuromaterialDataSprite));
	//今回は書き込んでみる
	kuromaterialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 0.1f);
	kuromaterialDataSprite->endbleLighting = false;
	kuromaterialDataSprite->uvTransform = Identity();


	//どこいれればいいの(汗)//
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = CreateDepthStencilTextureResource(graphics.get()->GetDevice(), kClientWidth, kClientHeight);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = CreateDescriptorHeep(graphics.get()->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//format。基本的にresourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先端にDSVを作る
	graphics.get()->GetDevice()->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


	//DXCの初期化//

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));


	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));



	//RootSignatureを生成する//

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRange//

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//1から始まる
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter//

	//RootParameter
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで行う
	rootParameter[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTebleを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//VertexShaderで行う
	rootParameter[3].Descriptor.ShaderRegister = 1;//レジスタ番号0とバインド

	descriptionRootSignature.pParameters = rootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameter);//配列の長さ



	//Samplerの設定//

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありたっけMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);




	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(logStream, reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = graphics.get()->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));





	//InputLayoutの設定を行う//

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD"; // texcoord -> TEXCOORD
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	//BlendStateの設定を行う//

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE; // 複数のレンダーターゲットがある場合はTRUEに設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE; // 最初のレンダーターゲットのブレンドを有効にする
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; // ソースブレンドファクター: ソースアルファ
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // デスティネーションブレンドファクター: 1 - ソースアルファ
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // ブレンド演算: 加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; // アルファブレンドファクター (ストレートアルファの場合はONEが多い)
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // アルファブレンドファクター (ストレートアルファの場合はZEROが多い)
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // すべてのカラーチャンネルが書き込まれることを確認

	//RasterizerStateの設定を行う//

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//ShaderをCompileする//

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(logStream, L"Object3D.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(logStream, L"Object3D.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);


	//DepthStencilStateの設定//

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能の有効化
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOを生成する//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;//BlenderState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;


	//作成したらPSOに代入、DSCのFormatを設定する//
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = graphics.get()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));






	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(uint32_t) * 6); ;

	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;

	//Objectの描画//
	std::vector<std::vector<bool>> map = LoadBoolMapFromCSV("resources/MapData.csv");
	const int blockHeight = 10;
	const int blockWidht = 50;
	Model* block[blockHeight][blockWidht];
	ModelData blockModel = LoadObjFile("resources/block", "block.obj");
	for (int i = 0; i < blockHeight; i++) {
		for (int j = 0; j < blockWidht; j++) {
			block[i][j] = new Model(blockModel);
			block[i][j]->CreateModel(graphics.get()->GetDevice());
			block[i][j]->SetTransform(Transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1.0f + 2.0f * j,1.0f + 2.0f * i,0.0f } });
			if (!map[i][j]) {
				block[i][j]->SetAlive(false);
			}
		}
	}

	Model* player;
	ModelData playerModel = LoadObjFile("resources/Player", "Player.obj");
	player = new Model(playerModel);
	player->CreateModel(graphics.get()->GetDevice());

	Model* enemy;
	ModelData enemyModel = LoadObjFile("resources/Enemy", "Enemy.obj");
	enemy = new Model(enemyModel);
	enemy->CreateModel(graphics.get()->GetDevice());

	const int kNumParticles = 10;
	Model* deth[kNumParticles];
	ModelData dethModel = LoadObjFile("resources/DaethParticle", "daethParticle.obj");
	for (int i = 0; i < kNumParticles; i++) {
		deth[i] = new Model(dethModel);
		deth[i]->CreateModel(graphics.get()->GetDevice());
	}
	Model* skydome;
	ModelData skydomeModel = LoadObjFile("resources/skydome", "skydome.obj");
	skydome = new Model(skydomeModel);
	skydome->CreateModel(graphics.get()->GetDevice());

	Model* Title;
	ModelData TitleModel = LoadObjFile("resources/Title", "Title.obj");
	Title = new Model(TitleModel);
	Title->CreateModel(graphics.get()->GetDevice());

	Model* kuro;
	ModelData kuroModel = LoadObjFile("resources/kuro", "kuro.obj");
	kuro = new Model(kuroModel);
	kuro->CreateModel(graphics.get()->GetDevice());

	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(DirectionalLight));

	DirectionalLight* directinalLightData = nullptr;

	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));

	directinalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directinalLightData->direction = { 0.0f,-1.0f,0.0f };
	directinalLightData->intensity = 1.0f;


	//Textureを読み込んで転送する//-------------------
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(graphics.get()->GetDevice(), metaData);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mipImages, graphics.get()->GetDevice(), command.get()->GetCommandList());
	//実際にShaderResourceView
	//meteDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
	graphics.get()->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	//二枚目のTextureを読み込む---------------------
	DirectX::ScratchImage mapImages2 = LoadTexture(blockModel.material.textureDilePath);
	const DirectX::TexMetadata& metaData2 = mapImages2.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = CreateTextureResource(graphics.get()->GetDevice(), metaData2);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 = UploadTextureData(textureResource2.Get(), mapImages2, graphics.get()->GetDevice(), command.get()->GetCommandList());
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metaData2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metaData2.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 2);
	graphics.get()->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	//3枚目のTextureを読み込む----------------------
	DirectX::ScratchImage playerImages = LoadTexture(playerModel.material.textureDilePath);
	const DirectX::TexMetadata& playerMetaData = playerImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> playerTextureResource = CreateTextureResource(graphics.get()->GetDevice(), playerMetaData);
	Microsoft::WRL::ComPtr<ID3D12Resource> playerIntermediateResource = UploadTextureData(playerTextureResource.Get(), playerImages, graphics.get()->GetDevice(), command.get()->GetCommandList());
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC playerSrvDesc{};
	playerSrvDesc.Format = playerMetaData.format;
	playerSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	playerSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	playerSrvDesc.Texture2D.MipLevels = UINT(playerMetaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE playerTextureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 3);
	D3D12_GPU_DESCRIPTOR_HANDLE playerTextureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 3);
	graphics.get()->GetDevice()->CreateShaderResourceView(playerTextureResource.Get(), &playerSrvDesc, playerTextureSrvHandleCPU);

	//4枚目のTextureを読み込む---------------------------------------
	DirectX::ScratchImage enemyImages = LoadTexture(enemyModel.material.textureDilePath);
	const DirectX::TexMetadata& enemyMetaData = enemyImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource>enemyTextureResource = CreateTextureResource(graphics.get()->GetDevice(), enemyMetaData);
	Microsoft::WRL::ComPtr<ID3D12Resource>enemyIntermediateResource = UploadTextureData(enemyTextureResource.Get(), enemyImages, graphics.get()->GetDevice(), command.get()->GetCommandList());
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC enemySrvDesc{};
	enemySrvDesc.Format = enemyMetaData.format;
	enemySrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	enemySrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	enemySrvDesc.Texture2D.MipLevels = UINT(enemyMetaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE enemyTextureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 4);
	D3D12_GPU_DESCRIPTOR_HANDLE enemyTextureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 4);
	graphics.get()->GetDevice()->CreateShaderResourceView(enemyTextureResource.Get(), &enemySrvDesc, enemyTextureSrvHandleCPU);

	//5枚目のTextureを読み込む------------------
	DirectX::ScratchImage dethImages = LoadTexture(dethModel.material.textureDilePath);
	const DirectX::TexMetadata& dethMetaData = dethImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource>dethTextureResource = CreateTextureResource(graphics.get()->GetDevice(), dethMetaData);
	Microsoft::WRL::ComPtr<ID3D12Resource>dethIntermediateResource = UploadTextureData(dethTextureResource.Get(), dethImages, graphics.get()->GetDevice(), command.get()->GetCommandList());

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC dethSrvDesc{};
	dethSrvDesc.Format = dethMetaData.format;
	dethSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	dethSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	dethSrvDesc.Texture2D.MipLevels = UINT(dethMetaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE dethTextureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 5);
	D3D12_GPU_DESCRIPTOR_HANDLE dethTextureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 5);
	graphics.get()->GetDevice()->CreateShaderResourceView(dethTextureResource.Get(), &dethSrvDesc, dethTextureSrvHandleCPU);

	//6枚目のTextureを読み込む------------------
	DirectX::ScratchImage skyImages = LoadTexture(skydomeModel.material.textureDilePath);
	const DirectX::TexMetadata& skyMetaData = skyImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource>skyTextureResource = CreateTextureResource(graphics.get()->GetDevice(), skyMetaData);
	Microsoft::WRL::ComPtr<ID3D12Resource>skyIntermediateResource = UploadTextureData(skyTextureResource.Get(), skyImages, graphics.get()->GetDevice(), command.get()->GetCommandList());

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC skySrvDesc{};
	skySrvDesc.Format = skyMetaData.format;
	skySrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	skySrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	skySrvDesc.Texture2D.MipLevels = UINT(skyMetaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE skyTextureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 5);
	D3D12_GPU_DESCRIPTOR_HANDLE skyTextureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 5);
	graphics.get()->GetDevice()->CreateShaderResourceView(skyTextureResource.Get(), &skySrvDesc, skyTextureSrvHandleCPU);

	//7枚目のTextureを読み込む------------------
	DirectX::ScratchImage TitleImages = LoadTexture(TitleModel.material.textureDilePath);
	const DirectX::TexMetadata& TitleMetaData = TitleImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource>TitleTextureResource = CreateTextureResource(graphics.get()->GetDevice(), TitleMetaData);
	Microsoft::WRL::ComPtr<ID3D12Resource>TitleIntermediateResource = UploadTextureData(TitleTextureResource.Get(), TitleImages, graphics.get()->GetDevice(), command.get()->GetCommandList());

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC TitleSrvDesc{};
	TitleSrvDesc.Format = TitleMetaData.format;
	TitleSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	TitleSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	TitleSrvDesc.Texture2D.MipLevels = UINT(TitleMetaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE TitleTextureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 6);
	D3D12_GPU_DESCRIPTOR_HANDLE TitleTextureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 6);
	graphics.get()->GetDevice()->CreateShaderResourceView(TitleTextureResource.Get(), &TitleSrvDesc, TitleTextureSrvHandleCPU);
	
	
	//7枚目のTextureを読み込む------------------
	DirectX::ScratchImage kuroImages = LoadTexture(kuroModel.material.textureDilePath);
	const DirectX::TexMetadata& kuroMetaData = kuroImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource>kuroTextureResource = CreateTextureResource(graphics.get()->GetDevice(), kuroMetaData);
	Microsoft::WRL::ComPtr<ID3D12Resource>kuroIntermediateResource = UploadTextureData(kuroTextureResource.Get(), kuroImages, graphics.get()->GetDevice(), command.get()->GetCommandList());

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC kuroSrvDesc{};
	kuroSrvDesc.Format = TitleMetaData.format;
	kuroSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	kuroSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	kuroSrvDesc.Texture2D.MipLevels = UINT(kuroMetaData.mipLevels);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE kuroTextureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 7);
	D3D12_GPU_DESCRIPTOR_HANDLE kuroTextureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 7);
	graphics.get()->GetDevice()->CreateShaderResourceView(kuroTextureResource.Get(), &kuroSrvDesc, kuroTextureSrvHandleCPU);


	//ViewportとScissor（シザー）//
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シーザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;


	float rdius = 1.0f;
	bool useMonsterBall = true;

	player->SetTransform(Transform{ {1.0f,1.0f,1.0f}, { 0.0f,0.0f,0.0f },{4.0f,3.0f,-0.5f } });
	Vector3 velocity = { 0, 0, 0 };
	bool isOnGround = false;

	float playerSize = 2.0f;
	float playerHalfSize = playerSize / 2.0f;

	Coner p;
	enemy->SetTransform(Transform{ {1.0f,1.0f,1.0f}, { 0.0f,0.0f,0.0f },{10.0f,2.0f,-0.5f } });
	int enemySize = 2;

	for (int i = 0; i < kNumParticles; i++) {
		deth[i]->SetTransform(Transform{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f},{0.0f,0.0f,-0.5f} });
		deth[i]->SetAlive(false);
	}

	skydome->SetTransform(Transform{ {10.0f,10.0f,10.0f}, { 0.0f,0.0f,0.0f },{0.0f,0.0f,0.0f } });

	Title->SetTransform(Transform{ {1.0f,0.9f,1.0f}, { 0.0f,0.0f,0.0f },{0.0f,0.0f,-0.5f } });

	kuro->SetTransform(Transform{ {1.0f,0.9f,1.0f}, { 0.0f,0.0f,0.0f },{0.0f,0.0f,-0.5f } });


	float color = 0.0f;
	bool changeCount = false;

	bool ChangeScene = false;
	bool GameScene = false;

	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-50.0f} };
	Transform uvTransformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	Audio* audio = new Audio;
	int BGMHandle;
	audio->Initialize();
	BGMHandle = audio->Load("resources/Audio/BGM/sweet_pop.mp3");
	//audio->Play(BGMHandle, false, 0.5f);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(window.GetHwnd());
	ImGui_ImplDX12_Init(graphics.get()->GetDevice(),
		swapChain.get()->GetSwapChainDesc().BufferCount,
		rtvDesc.Format,
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

			input->Updata();

			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			//コマンドを積み込んで確定させる//
		   //これから書き込むバックバッファのインデックスを取得
			UINT backBufferIndex = swapChain.get()->GetSwapChain()->GetCurrentBackBufferIndex();
			//TransitionBarrierを張る//
			D3D12_RESOURCE_BARRIER barrier{};
			//今回バリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			//Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			//バリアを張る対象のリソース。現在のバックバッファに対して行う
			barrier.Transition.pResource = swapChain.get()->GetSwapChainResources(backBufferIndex);
			//遷移前（現在）のResouce
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//遷移後のResouce
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//TransitionBarrierを張る
			command.get()->GetCommandList()->ResourceBarrier(1, &barrier);


			//ゲーム処理

			//開発用UIの処理。実際に開発用UIを出す場合はここをゲ0無固有の処理に置き換える
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			//ImGui::ShowDemoWindow();
			if (ImGui::CollapsingHeader("Color")) {
				ImGui::DragFloat4("Color", &materialData->color.x, 1.0f);
				ImGui::ColorPicker4("Color", &materialData->color.x);
			}
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("camraTranslate", &cameraTransform.translate.x, 0.01f);
				ImGui::DragFloat3("camraRotate", &cameraTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("camraScale", &cameraTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Light"))
			{
				ImGui::DragFloat4("directinalLightData.Color", &directinalLightData->color.x, 0.01f);
				ImGui::DragFloat3("directinalLightData.Direction", &directinalLightData->direction.x, 0.01f);
				ImGui::DragFloat("directinalLightData.intensity", &directinalLightData->intensity, 0.01f);
			}
			if (ImGui::CollapsingHeader("SpriteUV"))
			{
				ImGui::DragFloat2("TranslateSpriteUV", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("ScaleSpriteUV", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("RotateSpriteUV", &uvTransformSprite.rotate.z);
			}
			ImGui::DragFloat4("matrial", &materialDataSprite->color.x, 0.01f, -10.0f, 10.0f);
			ImGui::DragFloat4("detgmatrial", &dethmaterialDataSprite->color.x, 0.01f, -10.0f, 10.0f);

			if (!GameScene)
			{

				if (input->PushKey(DIK_SPACE)) {
					changeCount = true;
				}
				if (changeCount) {
					color += 0.5f / 60.0f;
					
					if (color >= 1.0f) {
						color = 1.0f;
						ChangeScene = true;
						GameScene = true;
					}

				}
				Title->SetWvp(cameraTransform);
			}
			if (ChangeScene) {

				color -= 0.5f / 60.0f;
				if (color < 0.0f) {
					ChangeScene = false;
				}
			}
			if (GameScene && !ChangeScene) {
				Vector3 playerPos = player->GetTransform().translate;
				// 入力処理
				if (input->PressKey(DIK_A)) {
					velocity.x -= 1.0f / 60.0f;
				}
				if (input->PressKey(DIK_D)) {
					velocity.x += 1.0f / 60.0f;
				}
				if (input->PushKey(DIK_SPACE) && isOnGround) {
					velocity.y = 1.0f;
					isOnGround = false;
				}


				// 重力
				velocity.y -= 0.02f;
				velocity.x *= 0.95f;
				// トンネリング対策のため細かく分割移動
				int steps = 10;
				Vector3 move = velocity;

				for (int i = 0; i < steps; ++i) {
					Vector3 stepMove = {
						move.x / steps,
						move.y / steps,
						0.0f
					};

					// X方向（左右判定）
					playerPos.x += stepMove.x;
					if (stepMove.x > 0) {
						if (IsHitBlock({ playerPos.x + playerHalfSize, playerPos.y }, map)) {
							playerPos.x -= stepMove.x;
							velocity.x = 0;
						}
					}
					else if (stepMove.x < 0) {
						if (IsHitBlock({ playerPos.x - playerHalfSize, playerPos.y }, map)) {
							playerPos.x -= stepMove.x;
							velocity.x = 0;
						}
					}

					// Y方向（上下判定＆着地時の位置補正）
					playerPos.y += stepMove.y;
					if (stepMove.y > 0) {
						if (IsHitBlock({ playerPos.x, playerPos.y + playerHalfSize }, map)) {
							playerPos.y -= stepMove.y;
							velocity.y = 0;
						}
					}
					else if (stepMove.y < 0) {
						if (IsHitBlock({ playerPos.x, playerPos.y - playerHalfSize }, map)) {
							// ブロックのインデックス
							int blockY = static_cast<int>((playerPos.y - playerHalfSize - 1.0f) / 2.0f);
							// ブロックの上面Y座標
							float blockTop = 1.0f + 2.0f * blockY + 1.0f;

							// プレイヤーをブロックの上にピタッと乗せる
							playerPos.y = blockTop + playerHalfSize;

							velocity.y = 0;
							isOnGround = true;
						}
					}
				}

				// 座標反映
				player->SetPos(playerPos);

				// カメラ追従（必要に応じて）
				if (player->GetAlive()) {
					cameraTransform.translate = playerPos;
					cameraTransform.translate.z -= 50.0f;
				}

				if (Lengeh(enemy->GetTransform().translate - player->GetTransform().translate) <= enemySize) {
					player->SetAlive(false);
				}


				for (uint32_t i = 0; i < kNumParticles; ++i) {
					if (player->GetAlive())
					{
						deth[i]->SetPos(player->GetTransform().translate);
						deth[i]->SetAlive(false);
						dethmaterialDataSprite->color.w = 1.0f;
					}
					else {
						Vector3 velocity = { 0.1f, 0.0f, 0.0f };
						float angle = (2.0f * 3.14159265f / kNumParticles) * i;
						Matrix4x4 matrixRotation = RotationZ(angle);
						velocity = TransformDirection(velocity, matrixRotation);
						deth[i]->SetAlive(true);
						dethmaterialDataSprite->color.w -= 0.2f / 60.0f;
						deth[i]->SetPos(deth[i]->GetTransform().translate + velocity); // 中心からずらす
					}
				}


			
			}

			for (int i = 0; i < blockHeight; i++) {
				for (int j = 0; j < blockWidht; j++) {
					block[i][j]->SetWvp(cameraTransform);
				}
			}
			for (int i = 0; i < kNumParticles; i++) {
				deth[i]->SetWvp(cameraTransform);
			}
			player->SetWvp(cameraTransform);
			enemy->SetWvp(cameraTransform);
			skydome->SetWvp(cameraTransform);
			kuro->SetWvp(cameraTransform);
			kuromaterialDataSprite->color.w = color;
			Matrix4x4 uvTransformMatrix = Scale(uvTransformSprite.scale);
			uvTransformMatrix = Multiply(uvTransformMatrix, Rotation(uvTransformSprite.rotate));
			uvTransformMatrix = Multiply(uvTransformMatrix, Translation(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;
			///-------------------------------
			///描画
			/// -------------------------------

			//描画先のRTVを設定する
			command.get()->GetCommandList()->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);

			//指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
			command.get()->GetCommandList()->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

			//DSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			command.get()->GetCommandList()->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

			command.get()->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			//ImGuiの内部コマンドを生成
			ImGui::Render();

			//コマンドを積む//

			ID3D12DescriptorHeap* descriptorHeeps[] = { descriptorHeap.get()->GetSrvDescriptorHeap() };
			command.get()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeeps);

			command.get()->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());//PSOを設定
			//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
			command.get()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			command.get()->GetCommandList()->IASetIndexBuffer(&indexBufferView);//IBVを設定


			command.get()->GetCommandList()->RSSetViewports(1, &viewport);//Viewportを設定
			command.get()->GetCommandList()->RSSetScissorRects(1, &scissorRect);//Sxirssorを設定
			//RootSignatureを設定。POSに設定しているけど別途設定が必要
			command.get()->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());
			if (!GameScene) {
				//skydomeの描画
				command.get()->GetCommandList()->IASetVertexBuffers(0, 1, Title->GetVertexBufferView());//VBVを設定
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, Title->GetWvpDataResource()->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TitleTextureSrvHandleGPU);
				command.get()->GetCommandList()->DrawInstanced(UINT(Title->GetModelData().vertices.size()), 1, 0, 0);

			}
			
				command.get()->GetCommandList()->IASetVertexBuffers(0, 1, kuro->GetVertexBufferView());//VBVを設定
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, kuromaterialResourceSprite->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, kuro->GetWvpDataResource()->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, kuroTextureSrvHandleGPU);
				command.get()->GetCommandList()->DrawInstanced(UINT(kuro->GetModelData().vertices.size()), 1, 0, 0);

			

			if (GameScene) {
				//skydomeの描画
				command.get()->GetCommandList()->IASetVertexBuffers(0, 1, skydome->GetVertexBufferView());//VBVを設定
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, skydome->GetWvpDataResource()->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, skyTextureSrvHandleGPU);
				command.get()->GetCommandList()->DrawInstanced(UINT(skydome->GetModelData().vertices.size()), 1, 0, 0);

				//objectの描画
				command.get()->GetCommandList()->IASetVertexBuffers(0, 1, block[0][0]->GetVertexBufferView());//VBVを設定
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
				for (int i = 0; i < blockHeight; i++) {
					for (int j = 0; j < blockWidht; j++) {
						if (block[i][j]->GetAlive()) {
							command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, block[i][j]->GetWvpDataResource()->GetGPUVirtualAddress());
							command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU2);
							command.get()->GetCommandList()->DrawInstanced(UINT(block[i][j]->GetModelData().vertices.size()), 1, 0, 0);
						}
					}
				}

				//Enemyの描画
				command.get()->GetCommandList()->IASetVertexBuffers(0, 1, enemy->GetVertexBufferView());//VBVを設定
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, enemy->GetWvpDataResource()->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, enemyTextureSrvHandleGPU);
				command.get()->GetCommandList()->DrawInstanced(UINT(enemy->GetModelData().vertices.size()), 1, 0, 0);

				//Playerの描画
				command.get()->GetCommandList()->IASetVertexBuffers(0, 1, player->GetVertexBufferView());//VBVを設定
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, player->GetWvpDataResource()->GetGPUVirtualAddress());
				command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, playerTextureSrvHandleGPU);
				if (player->GetAlive()) {
					command.get()->GetCommandList()->DrawInstanced(UINT(player->GetModelData().vertices.size()), 1, 0, 0);
				}

				//パーティクルの描画
				command.get()->GetCommandList()->IASetVertexBuffers(0, 1, deth[0]->GetVertexBufferView());//VBVを設定
				command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, dethmaterialResourceSprite->GetGPUVirtualAddress());

				for (int i = 0; i < kNumParticles; i++) {
					if (deth[i]->GetAlive()) {
						command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, deth[i]->GetWvpDataResource()->GetGPUVirtualAddress());
						command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, dethTextureSrvHandleGPU);
						command.get()->GetCommandList()->DrawInstanced(UINT(deth[i]->GetModelData().vertices.size()), 1, 0, 0);
					}
				}
			}


			//ImGuiの描画コマンド
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command.get()->GetCommandList());


			//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
			//今回RenderTargetからPresentにする
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//TransitionBarrierを張る
			command.get()->GetCommandList()->ResourceBarrier(1, &barrier);


			//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
			hr = command.get()->GetCommandList()->Close();
			assert(SUCCEEDED(hr));


			//コマンドをキックする//


			//GPU2コマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { command.get()->GetCommandList() };
			command.get()->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
			//GPUとOSに画面の交換を行うよう通知する
			swapChain.get()->GetSwapChain()->Present(1, 0);

			//GPUにSignalを送る//

			//Fenceの値を更新
			fenceValue++;
			hr = command.get()->GetCommandQueue()->Signal(fence.Get(), fenceValue);
			assert(SUCCEEDED(hr));

			//Fenceの値が更新されるまで待つ
			if (fence->GetCompletedValue() < fenceValue) {
				hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
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