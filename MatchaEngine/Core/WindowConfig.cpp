#include "WindowConfig.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT WindowConfig::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
	//メッセージに応じてゲーム固有の処理を行う
#endif
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリ終了を伝える
		PostQuitMessage(0);
		return 0;
	// Alt+Enterでフルスクリーン切り替え
	case WM_SYSKEYDOWN:
		if (wparam == VK_RETURN && (lparam & (1 << 29))) {
			// Alt+Enterが押された場合
			// 注意: この実装では静的なインスタンスが必要
			// 実際の使用時は適切なインスタンスアクセス方法を使用
			return 0;
		}
		break;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);

}



void WindowConfig::SetWindowData(const int32_t kClientWidth, const int32_t kClientHeight)
{
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

	//ウィンドウサイズを表示する構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズをwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	ClientArea_.x = wrc.right - wrc.left;
	ClientArea_.y = wrc.bottom - wrc.top;

	//ウィンドウの作成//
	hwnd = CreateWindow(
		wc.lpszClassName, //利用するクラス名
		L"LE2B_29_ワタナベ_マサト_ガンダッシュ",//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//よく見るウィンドウスタイル
		CW_USEDEFAULT,//表示X座標（Windowsに任せる）
		CW_USEDEFAULT,//表示Y座標（Windowsに任せる）
		ClientArea_.x,//ウィンドウ横幅
		ClientArea_.y,//ウィンドウ縦幅
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		wc.hInstance,//インスタンスハンドル
		nullptr);

	
}

void WindowConfig::DrawWindow(const int32_t kClientWidth, const int32_t kClientHeight)
{
	SetWindowData(kClientWidth, kClientHeight);
	//ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

}

void WindowConfig::Finalize() {
	//ウィンドウを破棄する
	DestroyWindow(hwnd);
	//ウィンドウクラスの登録解除
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	//COMの終了処理
	CoUninitialize();
}

bool WindowConfig::ProcessMassage()
{
	MSG msg{};
	//windowにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	
	if(msg.message == WM_QUIT) {
		return true;
	}


	return false;
}

void WindowConfig::ToggleFullscreen()
{
	SetFullscreen(!isFullscreen);
}

void WindowConfig::SetFullscreen(bool fullscreen)
{
	if (isFullscreen == fullscreen) {
		return; // 既に同じ状態の場合は何もしない
	}

	if (fullscreen) {
		// フルスクリーンに切り替え
		// 現在のウィンドウ情報を保存
		GetWindowRect(hwnd, &windowedRect);
		windowedStyle = GetWindowLong(hwnd, GWL_STYLE);

		// モニター情報を取得
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMonitor, &mi);

		// ウィンドウスタイルを変更（境界線なし）
		SetWindowLong(hwnd, GWL_STYLE, windowedStyle & ~(WS_CAPTION | WS_THICKFRAME));

		// ウィンドウを画面全体に拡張
		SetWindowPos(hwnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		isFullscreen = true;
	}
	else {
		// ウィンドウモードに戻す
		// 保存したスタイルを復元
		SetWindowLong(hwnd, GWL_STYLE, windowedStyle);

		// 保存した位置とサイズを復元
		SetWindowPos(hwnd, NULL,
			windowedRect.left, windowedRect.top,
			windowedRect.right - windowedRect.left,
			windowedRect.bottom - windowedRect.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		isFullscreen = false;
	}
}
