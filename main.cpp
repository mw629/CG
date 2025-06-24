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
#include "Matrial.h"

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


DirectX::ScratchImage LoadTexture(const std::string& filePath) {

	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミニマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{

	//metadataを基にResorceの設定

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの縦
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);;//奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。１固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは二次元

	//利用するHeapの設定

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定を行う


	//Resourceを作成

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//データ転送される設定
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = GraphicsDevice::CreateBufferResource(device, intermediateSize);

	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());


	//Tetureへの転送後は利用できるよう。D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;

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

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriprtorSize, uint32_t index) {

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriprtorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriprtorSize, uint32_t index) {

	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriprtorSize * index);
	return handleGPU;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materiaData;//構築するMaterialData
	std::string line;//ファイルから読んだ一行を格納する
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパス
			materiaData.textureDilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materiaData;
}


ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//1行分の文字列を入れる変数
	VertexData Triangle[3]{};

	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			//面は三角形限定。そのほか未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されてるので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}

				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				Triangle[faceVertex] = { position ,texcoord ,normal };

			}
			modelData.vertices.push_back(Triangle[2]);
			modelData.vertices.push_back(Triangle[1]);
			modelData.vertices.push_back(Triangle[0]);
		}
		else if (identifier == "mtllib") {
			//makterialTemplateLibraryファイルの名前を取得する
			std::string materiaFilename;
			s >> materiaFilename;
			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す 
			modelData.material = LoadMaterialTemplateFile(directoryPath, materiaFilename);
		}
	}
	return modelData;

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
	//入力
	Input* input = new Input;

	//描画
	DebugCamera* debudCamera = new DebugCamera;
	std::unique_ptr<Matrial> matrial = std::make_unique<Matrial>();
	std::unique_ptr<Matrial> spriteMatrial = std::make_unique<Matrial>();
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
	renderTargetView.get()->CreateRenderTargetView(graphics.get()->GetDevice(), swapChain.get()->GetSwapChainResources(0),swapChain.get()->GetSwapChainResources(1), descriptorHeap.get()->GetRtvDescriptorHeap());








	//初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	hr = graphics.get()->GetDevice()->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));

	//FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);


	//Material用のResourceを作る//

	matrial->CreateMatrial(graphics->GetDevice(),false);

	spriteMatrial->CreateMatrial(graphics->GetDevice(),false);


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

	//Sprote用の頂点リソースを作る//

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(VertexData) * 6);

	//頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//リソースの先頭アドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);



	//頂点データを設定する//

	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//１枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	//2枚目の三角形
	vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };

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



	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//単位行列をかきこんでおく
	transformationMatrixDataSprite->World = IdentityMatrix();
	transformationMatrixDataSprite->WVP = IdentityMatrix();



	//Objectの描画//

	ModelData modelData = LoadObjFile("resources/obj", "axis.obj");

	//頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceObj = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(VertexData) * modelData.vertices.size());
	//頂点バッファービューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewObj{};
	//リソースの先頭アドレスから使う
	vertexBufferViewObj.BufferLocation = vertexResourceObj->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewObj.SizeInBytes = sizeof(VertexData) * modelData.vertices.size();
	//1頂点当たりのサイズ
	vertexBufferViewObj.StrideInBytes = sizeof(VertexData);

	//頂点リソースにデータを書き込む
	VertexData* vertexDataObj = nullptr;
	vertexResourceObj->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataObj));
	std::memcpy(vertexDataObj, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());


	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceObj = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataObj = nullptr;
	//書き込むためのアドレスを取得
	transformationMatrixResourceObj->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataObj));
	//単位行列をかきこんでおく
	transformationMatrixDataObj->WVP = IdentityMatrix();
	transformationMatrixDataObj->World = IdentityMatrix();




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

	Microsoft::WRL::ComPtr<ID3D12Resource> directinalLightResource = GraphicsDevice::CreateBufferResource(graphics.get()->GetDevice(), sizeof(DirectionalLight));

	DirectionalLight* directinalLightData = nullptr;

	directinalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directinalLightData));

	directinalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directinalLightData->direction = { 0.0f,-1.0f,0.0f };
	directinalLightData->intensity = 1.0f;




	//Textureを読み込んで転送する//
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResource(graphics.get()->GetDevice(), metaData);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.Get(), mipImages, graphics.get()->GetDevice(), command.get()->GetCommandList());
	//二枚目のTextureを読み込む
	DirectX::ScratchImage mapImages2 = LoadTexture(modelData.material.textureDilePath);
	const DirectX::TexMetadata& metaData2 = mapImages2.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = CreateTextureResource(graphics.get()->GetDevice(), metaData2);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 = UploadTextureData(textureResource2.Get(), mapImages2, graphics.get()->GetDevice(), command.get()->GetCommandList());

	//実際にShaderResourceView

	//meteDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);
	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metaData2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metaData2.mipLevels);


	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 1);
	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(descriptorHeap.get()->GetSrvDescriptorHeap(), descriptorHeap.get()->GetDescriptorSizeSRV(), 2);

	graphics.get()->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	graphics.get()->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);


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

	Transform camraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-5.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
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
			
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("camraTranslate", &camraTransform.translate.x, 0.01f);
				ImGui::DragFloat3("camraRotate", &camraTransform.rotate.x, 0.01f);
				ImGui::DragFloat3("camraScale", &camraTransform.scale.x, 0.01f);
			}
			if (ImGui::CollapsingHeader("Sprite")) {
				ImGui::DragFloat3("TranslateSprite", &transformSprite.translate.x, 0.01f);
				ImGui::DragFloat3("RotateSprite", &transformSprite.rotate.x, 0.01f);
				ImGui::DragFloat3("ScaleSprite", &transformSprite.scale.x, 0.01f);
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
			ImGui::Text("move");
			ImGui::Text("x:AD\ny:WS\nz:SHIFT + WS\n");
			ImGui::Text("rotate");
			ImGui::Text("x:LEFT,RIGHT\ny:UP,DOEN\nz:QE\n");




			//transformShpere.rotate.y += 0.1f;


			//*wvpData = camera.MakeWorldViewProjectionMatrix(transform, camraTransform);
			//*transformationMatrixDataSprite = camera.MakeWorldViewProjectionMatrix(transformSprite, camraTransform);
			debudCamera->Update(*input);

			Matrix4x4 viewMatrix = debudCamera->GetViewMatrix();


			Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);

			Matrix4x4 worldMatrixObj = MakeAffineMatrix(transformObj.translate, transformObj.scale, transformObj.rotate);
			Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));
			*transformationMatrixDataObj = { worldViewProjectionMatrixObj,worldMatrixObj };

			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.translate, transformSprite.scale, transformSprite.rotate);
			Matrix4x4 worldViewProjectionMatrixSprite = MultiplyMatrix4x4(worldMatrixSprite, MultiplyMatrix4x4(viewMatrix, projectionMatri));
			*transformationMatrixDataSprite = { worldViewProjectionMatrixSprite,worldMatrixSprite };

			Matrix4x4 worldMatrixShpere = MakeAffineMatrix(transformShpere.translate, transformShpere.scale, transformShpere.rotate);
			Matrix4x4 worldViewProjectionMatrixShpere = MultiplyMatrix4x4(worldMatrixShpere, MultiplyMatrix4x4(viewMatrix, projectionMatri));
			*transformationMatrixDataShpere = { worldViewProjectionMatrixShpere,worldMatrixShpere };


			Matrix4x4 uvTransformMatrix = Scale(uvTransformSprite.scale);
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Rotation(uvTransformSprite.rotate));
			uvTransformMatrix = MultiplyMatrix4x4(uvTransformMatrix, Translation(uvTransformSprite.translate));
			spriteMatrial.get()->GetMaterialData()->uvTransform = uvTransformMatrix;

			//描画先のRTVを設定する
			command.get()->GetCommandList()->OMSetRenderTargets(1, renderTargetView.get()->GetRtvHandles(backBufferIndex), false, nullptr);

			//指定した色で画面全体をクリアする
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
			command.get()->GetCommandList()->ClearRenderTargetView(*renderTargetView.get()->GetRtvHandles(backBufferIndex), clearColor, 0, nullptr);

			//DSVを設定する
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			command.get()->GetCommandList()->OMSetRenderTargets(1, renderTargetView.get()->GetRtvHandles(backBufferIndex), false, &dsvHandle);

			command.get()->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


			//ImGuiの内部コマンドを生成
			ImGui::Render();


			//コマンドを積む//



			ID3D12DescriptorHeap* descriptorHeeps[] = { descriptorHeap.get()->GetSrvDescriptorHeap() };
			command.get()->GetCommandList()->SetDescriptorHeaps(1, descriptorHeeps);



			command.get()->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());//PSOを設定
			command.get()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
			//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
			command.get()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			command.get()->GetCommandList()->IASetIndexBuffer(&indexBufferView);//IBVを設定


			command.get()->GetCommandList()->RSSetViewports(1, &viewport);//Viewportを設定
			command.get()->GetCommandList()->RSSetScissorRects(1, &scissorRect);//Sxirssorを設定
			//RootSignatureを設定。POSに設定しているけど別途設定が必要
			command.get()->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directinalLightResource->GetGPUVirtualAddress());

			//スプライトの描画	
			command.get()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);//VBVを設定
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteMatrial.get()->GetMaterialResource()->GetGPUVirtualAddress());
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

			//objectの描画
			command.get()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewObj);//VBVを設定
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteMatrial.get()->GetMaterialResource()->GetGPUVirtualAddress());
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceObj->GetGPUVirtualAddress());
			command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			command.get()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

			//球の描画
			command.get()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewShpere);//VBVを設定
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteMatrial.get()->GetMaterialResource()->GetGPUVirtualAddress());
			command.get()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceShpere->GetGPUVirtualAddress());
			if (!useMonsterBall) {
				command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			}
			else {
				command.get()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU2);
			}
			//描画
			//commandList->DrawInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0);


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