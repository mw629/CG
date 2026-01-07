#include "TitleScene.h"
#include <cmath>

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

	// 初期位置を画面外（上）に設定
	titleCurrentY_ = -200.0f;
	titleVelocityY_ = 0.0f;
	titleLanded_ = false;
	Vector2 spritePos[2] = { {384.0f, titleCurrentY_}, {896.0f, titleCurrentY_ + 128.0f} };

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

	int magou = texture.get()->CreateTexture("resources/magou.png");
	se_.get()->Initialize(magou);
	Vector2 overlayPos_[2] = { {0.0f, 656.0f}, {128.0f, 720.0f} };
	se_.get()->SetSize(overlayPos_[0], overlayPos_[1]);
	se_->SettingWvp();
	se_.get()->SetBlend(BlendMode::kBlendModeNormal);

	skyDome_ = std::make_unique<SkyDome>();
	skyDome_.get()->Initialize();

	// 決定効果音のロード
	decideSE_ = Audio::Load("resources/Audio/SE/click.mp3");
}

void TitleScene::Update()
{

	skyDome_.get()->Update();

	// タイトル落下・バウンドアニメーション
	if (!titleLanded_) {
		// 重力を加算
		titleVelocityY_ += titleGravity_;
		titleCurrentY_ += titleVelocityY_;

		// 着地判定
		if (titleCurrentY_ >= titleTargetY_) {
			titleCurrentY_ = titleTargetY_;
			// 反発（跳ね返り）
			titleVelocityY_ = -titleVelocityY_ * titleBounceRate_;

			// 速度が十分小さくなったら停止
			if (std::abs(titleVelocityY_) < 1.0f) {
				titleVelocityY_ = 0.0f;
				titleLanded_ = true;
			}
		}

		// スプライトの位置を更新
		Vector2 spritePos[2] = { {384.0f, titleCurrentY_}, {896.0f, titleCurrentY_ + 128.0f} };
		backImage_.get()->SetSize(spritePos[0], spritePos[1]);
		backImage_->SettingWvp();
	}

	if (Input::PushKey(DIK_SPACE)||GamePadInput::PressButton(XINPUT_GAMEPAD_A)) {
		// 決定効果音を再生
		if (decideSE_ >= 0) {
			Audio::Play(decideSE_, false,2.0f);
		}
		nextSceneID_ = SceneID::kStageSelect;
		sceneChangeRequest_ = true;
	}
}

void TitleScene::Draw()
{
	skyDome_.get()->Draw();

	Draw::DrawSprite(backImage_.get());
	Draw::DrawSprite(SpaceSprite_.get());
	Draw::DrawSprite(se_.get());

}
