#include "GameApplication.h"


GameApplication::GameApplication(int32_t kClientWidth, int32_t kClientHeight)
{
	engine = std::make_unique<Engine>(kClientWidth, kClientHeight);
	engine.get()->Setting();
	sceneManager = std::make_unique<SceneManager>();
	editorManager = std::make_unique<EditorManager>();
}


void GameApplication::Run() {

	//ウィンドウのxが押されるまでループ
	while (true) {
		//windowにメッセージが来てたら最優先で処理させる
		if (WindowConfig::ProcessMassage()) {
			break;
		}

		engine.get()->NewFrame();

		editorManager->Update(engine.get());

#ifdef _USE_IMGUI
		// エディタモード: Play/Stop問わずInitialize・Drawは常に通す
		// Play状態でのみUpdate（ゲームロジック）を実行
		sceneManager.get()->Run();
#else
		// Release/Developmentでは常に実行
		sceneManager.get()->Run();
#endif

		engine.get()->EndFrame();
	}
	engine.get()->End();
}
