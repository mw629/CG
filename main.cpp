#include "externals/imgui/imgui_impl_win32.h"

#include "MatchaEngine/Engine.h"
#include "Game/SceneManager.h"

#pragma comment(lib,"dxcompiler.lib")

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


	//ウィンドウのxが押されるまでループ
	while (true) {
		//windowにメッセージが来てたら最優先で処理させる
		if (WindowConfig::ProcessMassage()) {
			break;
		}

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

	engine.get()->End();


	return 0;
}