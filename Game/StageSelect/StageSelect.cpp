#include "StageSelect.h"
#include "../Game/GameScene/GameScene.h"

float StageSelect::EaseOutCubic(float t)
{
	return 1.0f - std::pow(1.0f - t, 3.0f);
}

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

	// 9ステージ分のスプライト作成
	for (int i = 0; i < kMaxStage; i++) {
		sprite_[i] = std::make_unique<Sprite>();
		int textureHndle = texture.get()->CreateTexture("resources/UI/Stage/Stage" + std::to_string(i + 1) + ".png");
		sprite_[i]->Initialize(textureHndle);
		sprite_[i]->CreateSprite();

		// 初期位置は後でUpdateで設定するため、仮の値
		Vector2 spritePos[2] = { {512.0f, 264.0f}, {640.0f, 392.0f} };
		sprite_[i].get()->SetSize(spritePos[0], spritePos[1]);
		sprite_[i]->SettingWvp();
	}

	// 決定効果音のロード
	decideSE_ = Audio::Load("resources/Audio/SE/click.mp3");

	int textureHndle = texture.get()->CreateTexture("resources/UI/SPACE.png");
	SpaceSprite_->Initialize(textureHndle);
	SpaceSprite_->CreateSprite();
	Vector2 spritePos[2] = { {512.0f, 428.0f}, {768.0f, 492.0f} };
	SpaceSprite_.get()->SetSize(spritePos[0], spritePos[1]);
	SpaceSprite_->SettingWvp();

	// 初期オフセットを設定
	displayOffset_ = 0.0f;
	targetOffset_ = 0.0f;
}

void StageSelect::Update()
{
	// 入力処理
	if (Input::PushKey(DIK_D)) {
		stageNum_++;
	}
	if (Input::PushKey(DIK_A)) {
		stageNum_--;
	}

	// ステージ番号のクランプ (0?8)
	if (stageNum_ < 0) {
		stageNum_ = 0;
	}
	if (stageNum_ > kMaxStage - 1) {
		stageNum_ = kMaxStage - 1;
	}

	// 目標オフセットを更新（選択中のステージが中央に来るように）
	targetOffset_ = static_cast<float>(stageNum_) * kSpriteWidth;

	// イージングでオフセットを更新
	float diff = targetOffset_ - displayOffset_;
	displayOffset_ += diff * easingSpeed_;

	// 微小な差は丸める
	if (std::abs(diff) < 0.1f) {
		displayOffset_ = targetOffset_;
	}

	// 決定処理
	if (Input::PushKey(DIK_SPACE)) {
		if (decideSE_ >= 0) {
			Audio::Play(decideSE_, false, 2.0f);
		}
		sceneChangeRequest_ = true;
		nextSceneID_ = SceneID::kGame;
		GameScene::SetStage(stageNum_);
	}

	// スプライトの位置と色を更新
	for (int i = 0; i < kMaxStage; i++) {
		// 各スプライトのX位置を計算（中央からのオフセット）
		float posX = kCenterX + (i * kSpriteWidth) - displayOffset_;

		// スプライトサイズ（幅128）
		float halfWidth = 64.0f;
		float halfHeight = 64.0f;

		Vector2 spritePos[2] = {
			{posX - halfWidth, 264.0f},
			{posX + halfWidth, 392.0f}
		};
		sprite_[i].get()->SetSize(spritePos[0], spritePos[1]);
		sprite_[i]->SettingWvp();

		// 選択中のステージは赤色、それ以外は白色
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (i == stageNum_) {
			color = { 1.0f, 0.0f, 0.0f, 1.0f };
		}
		sprite_[i].get()->GetMatrial()->SetColor(color);
	}

	skyDome_.get()->Update();
}

void StageSelect::Draw()
{
	skyDome_.get()->Draw();

	// 画面内に表示されるスプライトのみ描画（最適化）
	for (int i = 0; i < kMaxStage; i++) {
		float posX = kCenterX + (i * kSpriteWidth) - displayOffset_;

		// 画面内にあるスプライトのみ描画
		if (posX > -kSpriteWidth && posX < 1280.0f + kSpriteWidth) {
			Draw::DrawSprite(sprite_[i].get());
		}
	}
	Draw::DrawSprite(SpaceSprite_.get());
}
