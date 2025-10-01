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

#include <memory> 

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/d3dx12.h"

#include "Game/SceneManager.h"

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
#include "MatchaEngine/Input/GamePadInput.h"


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


	std::unique_ptr<Engine> engine = std::make_unique<Engine>(1280, 720);
	engine.get()->Setting();

	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();
	sceneManager.get()->Initialize();

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


			engine.get()->NewFrame();

			engine.get()->Debug();

			sceneManager.get()->ImGui();

			sceneManager.get()->Update();


			//ゲーム処理

			//ImGuiの内部コマンドを生成
			ImGui::Render();

			sceneManager.get()->Draw();

			engine.get()->PostDraw();

			engine.get()->EndFrame();
		}
	}
	engine.get()->End();


	return 0;
}