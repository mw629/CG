#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include <windows.h>


<<<<<<< HEAD
//ウィンドウプロシージャ//

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSんい対して、アプリ終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準のメッセージ処理を行う
=======
//繧ｦ繧｣繝ｳ繝峨え繝励Ο繧ｷ繝ｼ繧ｸ繝｣//

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//繝｡繝・そ繝ｼ繧ｸ縺ｫ蠢懊§縺ｦ繧ｲ繝ｼ繝蝗ｺ譛峨・蜃ｦ逅・ｒ陦後≧
	switch (msg) {
		//繧ｦ繧｣繝ｳ繝峨え縺檎ｴ譽・＆繧後◆
	case WM_DESTROY:
		//OS繧薙＞蟇ｾ縺励※縲√い繝励Μ邨ゆｺ・ｒ莨昴∴繧・
		PostQuitMessage(0);
		return 0;
	}
	//讓呎ｺ悶・繝｡繝・そ繝ｼ繧ｸ蜃ｦ逅・ｒ陦後≧
>>>>>>> 
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


<<<<<<< HEAD
//出力ウィンドウに文字を出す//
=======
//蜃ｺ蜉帙え繧｣繝ｳ繝峨え縺ｫ譁・ｭ励ｒ蜃ｺ縺・/
>>>>>>> 

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

//ConvertString.cpp//

std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}



<<<<<<< HEAD
//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ウィンドウクラスの登録//

	WNDCLASS wc{};
	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	wc.lpszClassName = L"CGWindowClass";
	//インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを登録
	RegisterClass(&wc);


	//ウィンドウサイズの設定//

	//クライアント領域のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	//ウィンドウサイズを表示する構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズをwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);


	//ウィンドウの作成//

	HWND hwnd = CreateWindow(
		wc.lpszClassName, //利用するクラス名
		L"CG2",//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//よく見るウィンドウスタイル
		CW_USEDEFAULT,//表示X座標（Windowsに任せる）
		CW_USEDEFAULT,//表示Y座標（Windowsに任せる）
		wrc.right - wrc.left,//ウィンドウ横幅
		wrc.bottom - wrc.top,//ウィンドウ縦幅
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		wc.hInstance,//インスタンスハンドル
=======
//Windows繧｢繝励Μ縺ｧ縺ｮ繧ｨ繝ｳ繝医Μ繝ｼ繝昴う繝ｳ繝・main髢｢謨ｰ)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//繧ｦ繧｣繝ｳ繝峨え繧ｯ繝ｩ繧ｹ縺ｮ逋ｻ骭ｲ//

	WNDCLASS wc{};
	//繧ｦ繧｣繝ｳ繝峨え繝励Ο繧ｷ繝ｼ繧ｸ繝｣
	wc.lpfnWndProc = WindowProc;
	//繧ｦ繧｣繝ｳ繝峨え繧ｯ繝ｩ繧ｹ蜷・
	wc.lpszClassName = L"CGWindowClass";
	//繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ繝上Φ繝峨Ν
	wc.hInstance = GetModuleHandle(nullptr);
	//繧ｫ繝ｼ繧ｽ繝ｫ
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//繧ｦ繧｣繝ｳ繝峨え繧ｯ繝ｩ繧ｹ繧堤匳骭ｲ
	RegisterClass(&wc);


	//繧ｦ繧｣繝ｳ繝峨え繧ｵ繧､繧ｺ縺ｮ險ｭ螳・/

	//繧ｯ繝ｩ繧､繧｢繝ｳ繝磯伜沺縺ｮ繧ｵ繧､繧ｺ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	//繧ｦ繧｣繝ｳ繝峨え繧ｵ繧､繧ｺ繧定｡ｨ遉ｺ縺吶ｋ讒矩菴薙↓繧ｯ繝ｩ繧､繧｢繝ｳ繝磯伜沺繧貞・繧後ｋ
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//繧ｯ繝ｩ繧､繧｢繝ｳ繝磯伜沺繧貞・縺ｫ螳滄圀縺ｮ繧ｵ繧､繧ｺ繧蜘rc繧貞､画峩縺励※繧ゅｉ縺・
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);


	//繧ｦ繧｣繝ｳ繝峨え縺ｮ菴懈・//

	HWND hwnd = CreateWindow(
		wc.lpszClassName, //蛻ｩ逕ｨ縺吶ｋ繧ｯ繝ｩ繧ｹ蜷・
		L"CG2",//繧ｿ繧､繝医Ν繝舌・縺ｮ譁・ｭ・
		WS_OVERLAPPEDWINDOW,//繧医￥隕九ｋ繧ｦ繧｣繝ｳ繝峨え繧ｹ繧ｿ繧､繝ｫ
		CW_USEDEFAULT,//陦ｨ遉ｺX蠎ｧ讓呻ｼ・indows縺ｫ莉ｻ縺帙ｋ・・
		CW_USEDEFAULT,//陦ｨ遉ｺY蠎ｧ讓呻ｼ・indows縺ｫ莉ｻ縺帙ｋ・・
		wrc.right - wrc.left,//繧ｦ繧｣繝ｳ繝峨え讓ｪ蟷・
		wrc.bottom - wrc.top,//繧ｦ繧｣繝ｳ繝峨え邵ｦ蟷・
		nullptr,//隕ｪ繧ｦ繧｣繝ｳ繝峨え繝上Φ繝峨Ν
		nullptr,//繝｡繝九Η繝ｼ繝上Φ繝峨Ν
		wc.hInstance,//繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ繝上Φ繝峨Ν
>>>>>>> 
		nullptr);


	//DebugLayer//
#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
<<<<<<< HEAD
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
=======
		//繝・ヰ繝・げ繝ｬ繧､繝､繝ｼ繧呈怏蜉ｹ蛹悶☆繧・
		debugController->EnableDebugLayer();
		//縺輔ｉ縺ｫGPU蛛ｴ縺ｧ繧ゅメ繧ｧ繝・け繧定｡後≧繧医≧縺ｫ縺吶ｋ
>>>>>>> 
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif  



<<<<<<< HEAD
	//ウィンドウを表示する
=======
	//繧ｦ繧｣繝ｳ繝峨え繧定｡ｨ遉ｺ縺吶ｋ
>>>>>>> 
	ShowWindow(hwnd, SW_SHOW);



<<<<<<< HEAD
	//DXGIFactoryの生成//

	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	//HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違ってるか、
	//どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));


	//使用するアダプタ(GPU)を決定する//

	//使用するアダプタ用の変数。最初にnullptrを入れておく
	IDXGIAdapter4* useAdapter = nullptr;
	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; i++) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用!
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力、wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;//ソフトウェアアダプタの場合は見なかったことにする
	}
	//適切なアダプタがみつからなかったので起動できない
	assert(useAdapter != nullptr);


	//D3D12Deviceの生成//
	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0 };
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイスを作成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
=======
	//DXGIFactory縺ｮ逕滓・//

	//DXGI繝輔ぃ繧ｯ繝医Μ繝ｼ縺ｮ逕滓・
	IDXGIFactory7* dxgiFactory = nullptr;
	//HRESULT縺ｯWindows邉ｻ縺ｮ繧ｨ繝ｩ繝ｼ繧ｳ繝ｼ繝峨〒縺ゅｊ縲・
	// 髢｢謨ｰ縺梧・蜉溘＠縺溘°縺ｩ縺・°繧担UCCEEDED繝槭け繝ｭ縺ｧ蛻､螳壹〒縺阪ｋ
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//蛻晄悄蛹悶・譬ｹ譛ｬ逧・↑驛ｨ蛻・〒繧ｨ繝ｩ繝ｼ縺悟・縺溷ｴ蜷医・繝励Ο繧ｰ繝ｩ繝縺碁俣驕輔▲縺ｦ繧九°縲・
	//縺ｩ縺・↓繧ゅ〒縺阪↑縺・ｴ蜷医′螟壹＞縺ｮ縺ｧassert縺ｫ縺励※縺翫￥
	assert(SUCCEEDED(hr));


	//菴ｿ逕ｨ縺吶ｋ繧｢繝繝励ち(GPU)繧呈ｱｺ螳壹☆繧・/

	//菴ｿ逕ｨ縺吶ｋ繧｢繝繝励ち逕ｨ縺ｮ螟画焚縲よ怙蛻昴↓nullptr繧貞・繧後※縺翫￥
	IDXGIAdapter4* useAdapter = nullptr;
	//濶ｯ縺・・↓繧｢繝繝励ち繧帝ｼ繧
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
		DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; i++) {
		//繧｢繝繝励ち繝ｼ縺ｮ諠・ｱ繧貞叙蠕励☆繧・
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//蜿門ｾ励〒縺阪↑縺・・縺ｯ荳螟ｧ莠・
		//繧ｽ繝輔ヨ繧ｦ繧ｧ繧｢繧｢繝繝励ち縺ｧ縺ｪ縺代ｌ縺ｰ謗｡逕ｨ!
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//謗｡逕ｨ縺励◆繧｢繝繝励ち縺ｮ諠・ｱ繧偵Ο繧ｰ縺ｫ蜃ｺ蜉帙『string縺ｮ譁ｹ縺ｪ縺ｮ縺ｧ豕ｨ諢・
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;//繧ｽ繝輔ヨ繧ｦ繧ｧ繧｢繧｢繝繝励ち縺ｮ蝣ｴ蜷医・隕九↑縺九▲縺溘％縺ｨ縺ｫ縺吶ｋ
	}
	//驕ｩ蛻・↑繧｢繝繝励ち縺後∩縺､縺九ｉ縺ｪ縺九▲縺溘・縺ｧ襍ｷ蜍輔〒縺阪↑縺・
	assert(useAdapter != nullptr);


	//D3D12Device縺ｮ逕滓・//
	ID3D12Device* device = nullptr;
	//讖溯・繝ｬ繝吶Ν縺ｨ繝ｭ繧ｰ蜃ｺ蜉帷畑縺ｮ譁・ｭ怜・
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0 };
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//鬮倥＞鬆・↓逕滓・縺ｧ縺阪ｋ縺玖ｩｦ縺励※縺・￥
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//謗｡逕ｨ縺励◆繧｢繝繝励ち繝ｼ縺ｧ繝・ヰ繧､繧ｹ繧剃ｽ懈・
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//謖・ｮ壹＠縺滓ｩ溯・繝ｬ繝吶Ν縺ｧ繝・ヰ繧､繧ｹ縺檎函謌舌〒縺阪◆縺九ｒ遒ｺ隱・
		if (SUCCEEDED(hr)) {
			//逕滓・縺ｧ縺阪◆縺ｮ縺ｧ繝ｭ繧ｰ蜃ｺ蜉帙ｒ陦後▲縺ｦ繝ｫ繝ｼ繝励ｒ謚懊￠繧・
>>>>>>> 
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
<<<<<<< HEAD
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");//初期化完了ログを出す


	//エラー・警告、即ちに停止//
=======
	//繝・ヰ繧､繧ｹ縺ｮ逕滓・縺後≧縺ｾ縺上＞縺九↑縺九▲縺溘・縺ｧ襍ｷ蜍輔〒縺阪↑縺・
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");//蛻晄悄蛹門ｮ御ｺ・Ο繧ｰ繧貞・縺・


	//繧ｨ繝ｩ繝ｼ繝ｻ隴ｦ蜻翫∝叉縺｡縺ｫ蛛懈ｭ｢//
>>>>>>> 

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
<<<<<<< HEAD
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		

		//エラーと警告の抑制//

		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/question/69805245/directx-12-application-is-crashing-in-windows--11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		//抑制レベル
=======
		//繧・・縺・お繝ｩ繝ｼ譎ゅ↓豁｢縺ｾ繧・
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//繧ｨ繝ｩ繝ｼ譎ゅ↓豁｢縺ｾ繧・
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//隴ｦ蜻頑凾縺ｫ豁｢縺ｾ繧・
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);


		//繧ｨ繝ｩ繝ｼ縺ｨ隴ｦ蜻翫・謚大宛//

		//謚大宛縺吶ｋ繝｡繝・そ繝ｼ繧ｸ縺ｮID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11縺ｧ縺ｮDXGI繝・ヰ繝・げ繝ｬ繧､繝､繝ｼ縺ｨDX12繝・ヰ繝・げ繝ｬ繧､繝､繝ｼ縺ｮ逶ｸ莠剃ｽ懃畑繝舌げ縺ｫ繧医ｋ繧ｨ繝ｩ繝ｼ繝｡繝・そ繝ｼ繧ｸ
			//https://stackoverflow.com/question/69805245/directx-12-application-is-crashing-in-windows--11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		//謚大宛繝ｬ繝吶Ν
>>>>>>> 
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
<<<<<<< HEAD
		//指定したメッセージの表示を抑制
		infoQueue->PushStorageFilter(&filter);

		
		//解放
=======
		//謖・ｮ壹＠縺溘Γ繝・そ繝ｼ繧ｸ縺ｮ陦ｨ遉ｺ繧呈椛蛻ｶ
		infoQueue->PushStorageFilter(&filter);


		//隗｣謾ｾ
>>>>>>> 
		infoQueue->Release();
	}
#endif



<<<<<<< HEAD
	//CommandQueueを生成する//

	//コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//コマンドキューを生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));


	//CommandListを生成する//

	//コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator,
		nullptr, IID_PPV_ARGS(&commandList));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));


	//SwapChainを生成する//

	//スワップチェーンの生成する
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = kClientHeight;//画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画ターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら、中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
=======
	//CommandQueue繧堤函謌舌☆繧・/

	//繧ｳ繝槭Φ繝峨く繝･繝ｼ繧堤函謌舌☆繧・
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//繧ｳ繝槭Φ繝峨く繝･繝ｼ繧堤函謌舌′縺・∪縺上＞縺九↑縺九▲縺溘・縺ｧ襍ｷ蜍輔〒縺阪↑縺・
	assert(SUCCEEDED(hr));


	//CommandList繧堤函謌舌☆繧・/

	//繧ｳ繝槭Φ繝峨い繝ｭ繧ｱ繝ｼ繧ｿ繧堤函謌舌☆繧・
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//繧ｳ繝槭Φ繝峨い繝ｭ繧ｱ繝ｼ繧ｿ縺ｮ逕滓・縺後≧縺ｾ縺上＞縺九↑縺九▲縺溘・縺ｧ襍ｷ蜍輔〒縺阪↑縺・
	assert(SUCCEEDED(hr));

	//繧ｳ繝槭Φ繝峨Μ繧ｹ繝医ｒ逕滓・縺吶ｋ
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator,
		nullptr, IID_PPV_ARGS(&commandList));
	//繧ｳ繝槭Φ繝峨Μ繧ｹ繝医・逕滓・縺後≧縺ｾ縺上＞縺九↑縺九▲縺溘・縺ｧ襍ｷ蜍輔〒縺阪↑縺・
	assert(SUCCEEDED(hr));


	//SwapChain繧堤函謌舌☆繧・/

	//繧ｹ繝ｯ繝・・繝√ぉ繝ｼ繝ｳ縺ｮ逕滓・縺吶ｋ
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;//逕ｻ髱｢縺ｮ蟷・ゅえ繧｣繝ｳ繝峨え縺ｮ繧ｯ繝ｩ繧､繧｢繝ｳ繝磯伜沺繧貞酔縺倥ｂ縺ｮ縺ｫ縺励※縺翫￥
	swapChainDesc.Height = kClientHeight;//逕ｻ髱｢縺ｮ鬮倥＆縲ゅえ繧｣繝ｳ繝峨え縺ｮ繧ｯ繝ｩ繧､繧｢繝ｳ繝磯伜沺繧貞酔縺倥ｂ縺ｮ縺ｫ縺励※縺翫￥
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//濶ｲ縺ｮ蠖｢蠑・
	swapChainDesc.SampleDesc.Count = 1;//繝槭Ν繝√し繝ｳ繝励Ν縺励↑縺・
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//謠冗判繧ｿ繝ｼ繧ｲ繝・ヨ縺ｨ縺励※蛻ｩ逕ｨ縺吶ｋ
	swapChainDesc.BufferCount = 2;//繝繝悶Ν繝舌ャ繝輔ぃ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//繝｢繝九ち縺ｫ縺・▽縺励◆繧峨∽ｸｭ霄ｫ繧堤ｴ譽・
	//繧ｳ繝槭Φ繝峨く繝･繝ｼ縲√え繧｣繝ｳ繝峨え繝上Φ繝峨Ν縲∬ｨｭ螳壹ｒ貂｡縺励※逕滓・縺吶ｋ
>>>>>>> 
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc,
		nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));


<<<<<<< HEAD
	//DescriptorHeapを生成する

	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDescruptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//レンダーターゲットビュー用
	rtvDescriptorHeapDesc.NumDescriptors = 2;//ダブルバッファ用に2つ。多くても別に構わない
	hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescruptorHeap));
	//ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));


	//SwapChainからResourceを引っ張ってくる//

	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//うまく起動できなければ起動できない
=======
	//DescriptorHeap繧堤函謌舌☆繧・

	//繝・ぅ繧ｹ繧ｯ繝ｪ繝励ち繝偵・繝励・逕滓・
	ID3D12DescriptorHeap* rtvDescruptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//繝ｬ繝ｳ繝繝ｼ繧ｿ繝ｼ繧ｲ繝・ヨ繝薙Η繝ｼ逕ｨ
	rtvDescriptorHeapDesc.NumDescriptors = 2;//繝繝悶Ν繝舌ャ繝輔ぃ逕ｨ縺ｫ2縺､縲ょ､壹￥縺ｦ繧ょ挨縺ｫ讒九ｏ縺ｪ縺・
	hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescruptorHeap));
	//繝・ぅ繧ｹ繧ｯ繝ｪ繝励ち繝偵・繝励′菴懊ｌ縺ｪ縺九▲縺溘・縺ｧ襍ｷ蜍輔〒縺阪↑縺・
	assert(SUCCEEDED(hr));


	//SwapChain縺九ｉResource繧貞ｼ輔▲蠑ｵ縺｣縺ｦ縺上ｋ//

	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//縺・∪縺剰ｵｷ蜍輔〒縺阪↑縺代ｌ縺ｰ襍ｷ蜍輔〒縺阪↑縺・
>>>>>>> 
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));


<<<<<<< HEAD
	//RTVを作る//

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescruptorHeap->GetCPUDescriptorHandleForHeapStart();
	//RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//まず一つ目作る。1つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	//2つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);


	//コマンドを積み込んで確定させる//

	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	
	
	//TransitionBarrierを張るコード//

	//TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	//今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	//遷移前(現在)のResource
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//遷移後のResource
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);

	//描画先のRTVを設定する
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
	//指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
	
	
	//画面表示できるようにする//

	//画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	//今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrierを張る
	commandList->ResourceBarrier(1, &barrier);
	
	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
=======
	//RTV繧剃ｽ懊ｋ//

	//RTV縺ｮ險ｭ螳・
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//蜃ｺ蜉帷ｵ先棡繧担RGB縺ｫ螟画鋤縺励※譖ｸ縺崎ｾｼ繧
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2d繝・け繧ｹ繝√Ε縺ｨ縺励※譖ｸ縺崎ｾｼ繧
	//繝・ぅ繧ｹ繧ｯ繝ｪ繝励ち縺ｮ蜈磯ｭ繧貞叙蠕励☆繧・
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescruptorHeap->GetCPUDescriptorHandleForHeapStart();
	//RTV繧・縺､菴懊ｋ縺ｮ縺ｧ繝・ぅ繧ｹ繧ｯ繝ｪ繝励ち繧・縺､逕ｨ諢・
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//縺ｾ縺壻ｸ縺､逶ｮ菴懊ｋ縲・縺､逶ｮ縺ｯ譛蛻昴・縺ｨ縺薙ｍ縺ｫ菴懊ｋ縲ゆｽ懊ｋ蝣ｴ謇繧偵％縺｡繧峨〒謖・ｮ壹＠縺ｦ縺ゅ￡繧句ｿ・ｦ√′縺ゅｋ
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	//2縺､逶ｮ縺ｮ繝・ぅ繧ｹ繧ｯ繝ｪ繝励ち繝上Φ繝峨Ν繧貞ｾ励ｋ・郁・蜉帙〒・・
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2縺､逶ｮ繧剃ｽ懊ｋ
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);


	//Descriptor縺ｮ菴咲ｽｮ豎ｺ繧・/

	//rtvHandles[0] = rtvStartHandle;
	//rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize();


	//繧ｳ繝槭Φ繝峨ｒ遨阪∩霎ｼ繧薙〒遒ｺ螳壹＆縺帙ｋ//

	//縺薙ｌ縺九ｉ譖ｸ縺崎ｾｼ繧繝舌ャ繧ｯ繝舌ャ繝輔ぃ縺ｮ繧､繝ｳ繝・ャ繧ｯ繧ｹ繧貞叙蠕・
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	
	//TransitionBarrier繧貞ｼｵ繧・/

	//TransitionBarrer縺ｮ險ｭ螳・
	D3D12_RESOURCE_BARRIER barrier{};
	//莉雁屓縺ｮ繝舌Μ繧｢縺ｯTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//None縺ｫ縺励※縺翫￥
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//繝舌Μ繧｢繧貞ｼｵ繧句ｯｾ雎｡縺ｮ繝ｪ繧ｽ繝ｼ繧ｹ縲ら樟蝨ｨ縺ｮ繝舌ャ繧ｯ繝舌ャ繝輔ぃ縺ｫ蟇ｾ縺励※陦後≧
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	//驕ｷ遘ｻ蜑搾ｼ育樟蝨ｨ・峨・Resource
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//驕ｷ遘ｻ蠕後・Resource
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TRansitiionBarrier繧貞ｼｵ繧・
	commandList->ResourceBarrier(1, &barrier);


	//謠冗判蜈医・RTV繧定ｨｭ螳壹☆繧・
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
	//謖・ｮ壹＠縺溯牡縺ｧ逕ｻ髱｢蜈ｨ菴薙ｒ繧ｯ繝ｪ繧｢縺吶ｋ
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//髱偵▲縺ｽ縺・牡縲３GBA縺ｮ鬆・
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
	
	
	//逕ｻ髱｢縺ｫ謠上￥蜃ｦ逅・・縺吶∋縺ｦ邨ゅｏ繧翫∫判髱｢縺ｫ譏縺吶・縺ｧ縲∫憾諷九ｒ驕ｷ遘ｻ
	//莉雁屓縺ｯRenderTarget縺九ｉPresent縺ｫ縺吶ｋ
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitionBarrier
	commandList->ResourceBarrier(1, &barrier);

	//繧ｳ繝槭Φ繝峨Μ繧ｹ繝医・蜀・ｮｹ繧堤｢ｺ螳壹＆縺帙ｋ縲ゅ☆縺ｹ縺ｦ縺ｮ繧ｳ繝槭Φ繝峨ｒ遨阪ｓ縺ｧ縺九ｉClose縺吶ｋ縺薙→
>>>>>>> 
	hr = commandList->Close();
	assert(SUCCEEDED(hr));


<<<<<<< HEAD
	//コマンドをキックする//

	//GPU2コマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain->Present(1, 0);
	//次のフレーム用のコマンドリストを準備
=======
	//繧ｳ繝槭Φ繝峨ｒ繧ｭ繝・け縺吶ｋ//

	//GPU2繧ｳ繝槭Φ繝峨Μ繧ｹ繝医・螳溯｡後ｒ陦後ｏ縺帙ｋ
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);
	//GPU縺ｨOS縺ｫ逕ｻ髱｢縺ｮ莠､謠帙ｒ陦後≧繧医≧騾夂衍縺吶ｋ
	swapChain->Present(1, 0);
	//谺｡縺ｮ繝輔Ξ繝ｼ繝逕ｨ縺ｮ繧ｳ繝槭Φ繝峨Μ繧ｹ繝医ｒ貅門ｙ
>>>>>>> 
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));


<<<<<<< HEAD
	

	//メインループ//

	MSG msg{};
	//ウィンドウのxが押されるまでループ
	while (msg.message != WM_QUIT) {
		//windowにメッセージが来てたら最優先で処理させる
=======

	//繝｡繧､繝ｳ繝ｫ繝ｼ繝・/

	MSG msg{};
	//繧ｦ繧｣繝ｳ繝峨え縺ｮx縺梧款縺輔ｌ繧九∪縺ｧ繝ｫ繝ｼ繝・
	while (msg.message != WM_QUIT) {
		//window縺ｫ繝｡繝・そ繝ｼ繧ｸ縺梧擂縺ｦ縺溘ｉ譛蜆ｪ蜈医〒蜃ｦ逅・＆縺帙ｋ
>>>>>>> 
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
<<<<<<< HEAD
			//ゲーム処理
		}
	}

	//出力ウィンドウへの文字出力
=======
			//繧ｲ繝ｼ繝蜃ｦ逅・
		}
	}

	//蜃ｺ蜉帙え繧｣繝ｳ繝峨え縺ｸ縺ｮ譁・ｭ怜・蜉・
>>>>>>> 
	OutputDebugStringA("Hello,DirectX!\n");

	return 0;
}