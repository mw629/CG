#include "TitleScene.h"
#include <imgui.h>

void TitleScene::ImGui()
{
#ifdef _USE_IMGUI
ImGui::Text("TITLE");
#endif // _USE_IMGUI
}

void TitleScene::Initialize()
{
	sceneID_ = SceneID::Title;

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int backImageHandle = texture.get()->CreateTexture("resources/nightSky.png");

	spritePos[0] = { 0.0f,0.0f };
	spritePos[1] = { 1280.0f, 720.0f };



}

void TitleScene::Update()
{



	if (Input::PushKey(DIK_SPACE)||GamePadInput::PressButton(XINPUT_GAMEPAD_A)) {
		nextSceneID_ = SceneID::Game;
		sceneChangeRequest_ = true;
	}
}

void TitleScene::Draw()
{
}
