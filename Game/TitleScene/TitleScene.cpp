#include "TitleScene.h"
#include <imgui.h>

void TitleScene::ImGui()
{
	ImGui::Text("TITLE");
}

void TitleScene::Initialize()
{
	sceneID_ = SceneID::kTitle;

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int backImageHandle = texture.get()->CreateTexture("resources/UI/TITLE.png");

	backImage_ = std::make_unique<Sprite>();
	backImage_->Initialize(backImageHandle);
	backImage_->CreateSprite();
	Vector2 spritePos[2] = { {512.0f,328.0f}, {768.0f,392.0f} };

	backImage_.get()->SetSize(spritePos[0], spritePos[1]);
	backImage_->SettingWvp();

	skyDome_ = std::make_unique<SkyDome>();
	skyDome_.get()->Initialize();

}

void TitleScene::Update()
{

	

	skyDome_.get()->Update();

	if (Input::PushKey(DIK_SPACE)||GamePadInput::PressButton(XINPUT_GAMEPAD_A)) {
		nextSceneID_ = SceneID::kGame;
		sceneChangeRequest_ = true;
	}
}

void TitleScene::Draw()
{
	skyDome_.get()->Draw();

	Draw::DrawSprite(backImage_.get());

}
