#include "externals/imgui/imgui_impl_win32.h"

#include "MatchaEngine/Engine.h"
#include "Game/SceneManager.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<Engine> engine = std::make_unique<Engine>(1280, 720);
	engine.get()->Setting();

	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();
	sceneManager.get()->Initialize();

	while (true) {
		if (WindowConfig::ProcessMassage()) {
			break;
		}
		engine.get()->NewFrame();
		engine.get()->Debug();
		sceneManager.get()->ImGui();
		sceneManager.get()->Update();
#ifdef _USE_IMGUI
		ImGui::Render();
#endif 
		sceneManager.get()->Draw();

		engine.get()->PostDraw();

		engine.get()->EndFrame();
	}
	engine.get()->End();
	return 0;
}