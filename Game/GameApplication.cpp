#include "GameApplication.h"


GameApplication::GameApplication(int32_t kClientWidth, int32_t kClientHeight)
{
	engine = std::make_unique<Engine>(kClientWidth, kClientHeight);
	engine.get()->Setting();
	sceneManager = std::make_unique<SceneManager>();
}


void GameApplication::Run() {

	//ウィンドウのxが押されるまでループ
	while (true) {
		//windowにメッセージが来てたら最優先で処理させる
		if (WindowConfig::ProcessMassage()) {
			break;
		}

		engine.get()->NewFrame();

		engine.get()->Debug();

		sceneManager.get()->Run();

		engine.get()->EndFrame();
	}
	engine.get()->End();
}
