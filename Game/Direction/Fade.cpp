#include "Fade.h"


void Fade::Initialize(float fadeSpeed) {

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	int textureHndle = texture.get()->CreateTexture("resources/white64x64.png");

	overlay_ = std::make_unique<Sprite>();
	overlay_.get()->Initialize(textureHndle);

	overlay_.get()->GetMatrial()->SetColor(overlayColor_);
	overlay_.get()->SetSize(overlayPos_[0], overlayPos_[1]);
	overlay_->SettingWvp();
	overlay_.get()->SetBlend(BlendMode::kBlendModeNormal);

	fadeSpeed_ = fadeSpeed;
	fadeFream_ = 60.0f;
}

void Fade::Update() {

	float deltaAlpha = 1.0f / fadeFream_;

	switch (state_)
	{
	case Fade::kFadeIn:

		overlayColor_.w += deltaAlpha / fadeSpeed_;
		if (overlayColor_.w >= 1.0f) {
			overlayColor_.w = 1.0f;
			state_ = kNone;
			isFinish_ = true;
		}

		break;
	case Fade::kFadeOut:

		overlayColor_.w -= deltaAlpha / fadeSpeed_;
		if (overlayColor_.w <= 0.0f) {
			overlayColor_.w = 0.0f;
			state_ = kNone; // フェード完了
			isFinish_ = true;
		}
		break;
	default:
		break;
	}

	//更新処理
	overlay_.get()->GetMatrial()->SetColor(overlayColor_);
	overlay_.get()->SettingWvp();
}
void Fade::Draw() {
	Draw::DrawSprite(overlay_.get());
}



void Fade::FadeIn() {
	isFinish_ = false;

	state_ = kFadeIn;
	fadeFream_ = 60.0f;
	overlayColor_.w = 0.0f;
	overlay_.get()->GetMatrial()->SetColor(overlayColor_);
}


void Fade::FadeOut() {
	isFinish_ = false;

	state_ = kFadeOut;
	fadeFream_ = 60.0f;
	overlayColor_.w = 1.0f;
	overlay_.get()->GetMatrial()->SetColor(overlayColor_);
}

