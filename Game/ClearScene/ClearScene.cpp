#include "ClearScene.h"

ClearScene::~ClearScene()
{
}

void ClearScene::ImGui()
{
}

void ClearScene::Initialize()
{
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int textureHndle = texture.get()->CreateTexture("resources/UI/Clear.png");

	//スプライト作成
	sprite_->Initialize(textureHndle);
	sprite_->CreateSprite();
	Vector2 spritePos[2] = { {512.0f,328.0f}, {768.0f,392.0f} };
	sprite_.get()->SetSize(spritePos[0], spritePos[1]);
	sprite_->SettingWvp();

	textureHndle = texture.get()->CreateTexture("resources/UI/SPACE.png");

	//スプライト作成
	SpaceSprite_->Initialize(textureHndle);
	SpaceSprite_->CreateSprite();
	spritePos[0] = { 512.0f,428.0f };
	spritePos[1] = { 768.0f,492.0f };
	SpaceSprite_.get()->SetSize(spritePos[0], spritePos[1]);
	SpaceSprite_->SettingWvp();

}

void ClearScene::Update()
{
	if (Input::PushKey(DIK_SPACE)) {
		nextSceneID_ = SceneID::kTitle;
		sceneChangeRequest_ = true;
	}
}

void ClearScene::Draw()
{
	Draw::DrawSprite(sprite_.get());
	Draw::DrawSprite(SpaceSprite_.get());
}

