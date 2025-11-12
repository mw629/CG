#include "StageSelect.h"
#include "../Game/GameScene/GameScene.h"

void StageSelect::ImGui()
{
#ifdef USE_IMGUI
#endif
}

void StageSelect::Initialize()
{
	skyDome_ = std::make_unique<SkyDome>();
	skyDome_.get()->Initialize();

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int textureHndle = texture.get()->CreateTexture("resources/UI/Clear.png");
	Vector2 spritePos[2] = { {512.0f,328.0f}, {768.0f,392.0f} };
	for (int i = 0; i < 3; i++) {
		sprite_[i] = std::make_unique<Sprite>();
		//スプライト作成
		textureHndle = texture.get()->CreateTexture("resources/UI/Stage/Stage" + std::to_string(i + 1) + ".png");
		sprite_[i]->Initialize(textureHndle);
		sprite_[i]->CreateSprite();

		spritePos[0] = { 320.0f + (128 * 2 * i),264.0f };
		spritePos[1] = { 448.0f + (128 * 2 * i),392.0f };

		sprite_[i].get()->SetSize(spritePos[0], spritePos[1]);
		sprite_[i]->SettingWvp();
	}


	textureHndle = texture.get()->CreateTexture("resources/UI/SPACE.png");
	//スプライト作成
	SpaceSprite_->Initialize(textureHndle);
	SpaceSprite_->CreateSprite();
	spritePos[0] = { 512.0f,428.0f };
	spritePos[1] = { 768.0f,492.0f };
	SpaceSprite_.get()->SetSize(spritePos[0], spritePos[1]);
	SpaceSprite_->SettingWvp();


}

void StageSelect::Update()
{

	if (Input::PushKey(DIK_D)) {
		stageNum_++;
	}
	if (Input::PushKey(DIK_A)) {
		stageNum_--;
	}
	if (stageNum_ < 0) {
		stageNum_ = 0;
	}
	if (stageNum_ > 2) {
		stageNum_ = 2;
	}

	if (Input::PushKey(DIK_SPACE)) {
		sceneChangeRequest_ = true;
		nextSceneID_ = SceneID::kGame;
		GameScene::SetStage(stageNum_);
	}

	for (int i = 0; i < 3; i++) {

		Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
		if (i == stageNum_) {
			color = { 1.0f,0.0f,0.0f,1.0f };
		}

		sprite_[i].get()->GetMatrial()->SetColor(color);
	}


	skyDome_.get()->Update();
}

void StageSelect::Draw()
{
	skyDome_.get()->Draw();

	for (int i = 0; i < 3; i++) {
		Draw::DrawSprite(sprite_[i].get());
	}
	Draw::DrawSprite(SpaceSprite_.get());

}
