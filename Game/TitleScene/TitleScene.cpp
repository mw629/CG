#include "TitleScene.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

void TitleScene::ImGui()
{
#ifdef USE_IMGUI
	ImGui::Text("TITLE");
#endif
}

void TitleScene::Initialize()
{
	sceneID_ = SceneID::kTitle;

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int textureHndle = texture.get()->CreateTexture("resources/UI/TITLE.png");

	backImage_ = std::make_unique<Sprite>();
	backImage_->Initialize(textureHndle);
	backImage_->CreateSprite();
	Vector2 spritePos[2] = { {512.0f,328.0f}, {768.0f,392.0f} };

	backImage_.get()->SetSize(spritePos[0], spritePos[1]);
	backImage_->SettingWvp();

	textureHndle = texture.get()->CreateTexture("resources/UI/SPACE.png");

	//スプライト作成
	SpaceSprite_->Initialize(textureHndle);
	SpaceSprite_->CreateSprite();
	spritePos[0] = { 512.0f,428.0f };
	spritePos[1] = { 768.0f,492.0f };
	SpaceSprite_.get()->SetSize(spritePos[0], spritePos[1]);
	SpaceSprite_->SettingWvp();

	skyDome_ = std::make_unique<SkyDome>();
	skyDome_.get()->Initialize();

}

void TitleScene::Update()
{

	skyDome_.get()->Update();

	if (Input::PushKey(DIK_SPACE)||GamePadInput::PressButton(XINPUT_GAMEPAD_A)) {
		nextSceneID_ = SceneID::kStageSelect;
		sceneChangeRequest_ = true;
	}
}

void TitleScene::Draw()
{
	skyDome_.get()->Draw();

	Draw::DrawSprite(backImage_.get());
	Draw::DrawSprite(SpaceSprite_.get());

}
