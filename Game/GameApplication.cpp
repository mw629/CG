#include "GameApplication.h"

GameApplication::GameApplication(int32_t kClientWidth, int32_t kClientHeight) {
  engine = std::make_unique<Engine>(kClientWidth, kClientHeight);
  engine.get()->Setting();
  sceneManager = std::make_unique<SceneManager>();
  editorManager = std::make_unique<EditorManager>();
}

void GameApplication::Run() {

  // ウィンドウのxが押されるまでループ
  while (true) {
    // windowにメッセージが来てたら最優先で処理させる
    if (WindowConfig::ProcessMassage()) {
      break;
    }

    engine.get()->NewFrame();

    sceneManager.get()->PreUpdate();

    // 1. まずシーンの更新を実行（入力、オブジェクト・パーティクルの更新、emitフラグ確定）
    sceneManager.get()->Update();

#ifdef _USE_IMGUI
    // 2. シーンのImGui
    sceneManager.get()->ImGui();

    // 3. エディタ全体のImGuiおよびGameView描画（Update後の最新状態を描画）
    editorManager->Update(engine.get());

    // 4. メインシーン描画
    sceneManager.get()->Draw(*engine->draw);
#else
    // Release/Developmentでは直接シーンを描画
    sceneManager.get()->Draw(*engine->draw);
#endif

    engine.get()->EndFrame(
        [&]() { sceneManager.get()->DrawUI(*engine->draw); });
  }
  engine.get()->End();
}
