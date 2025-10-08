#include "TitleScene.h"
#include <imgui.h>

void TitleScene::ImGui()
{
	ImGui::Text("TITLE");
}

void TitleScene::Initialize()
{
	sceneID_ = SceneID::Title;

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int backImageHandle = texture.get()->CreateTexture("resources/nightSky.png");

	backImage_ = std::make_unique<Sprite>();
	backImage_->Initialize(texture->TextureData(backImageHandle));
	backImage_->CreateSprite();
	spritePos[0] = { 0.0f,0.0f };
	spritePos[1] = { 1280.0f, 720.0f };



}

void TitleScene::Update()
{

	backImage_.get()->SetSize(spritePos[0], spritePos[1]);
	backImage_->SettingWvp();


	if (Input::PushKey(DIK_SPACE)) {
		nextSceneID_ = SceneID::Game;
		sceneChangeRequest_ = true;
	}
}

void TitleScene::Draw()
{
	Draw::DrawSprite(backImage_.get());
}
