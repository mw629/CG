#include "Pause.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif


void Pause::ImGui() {
#ifdef USE_IMGUI
	int selectInt = static_cast<int>(menuCommand_);
	ImGui::DragInt("select", &selectInt, 1, kReturnToGame, kEnd);
	menuCommand_ = static_cast<MenuCommand>(selectInt);
#endif
}

void Pause::Initialize() {

	if (!isInitialized_) {
		std::unique_ptr<Texture> texture = std::make_unique<Texture>();
		int textureHndle = texture.get()->CreateTexture("resources/white64x64.png");
		dimming_.get()->Initialize(textureHndle);
		Vector2 overlayPos_[2] = { {0.0f, 0.0f}, {1280.0f, 720.0f} };
		dimming_.get()->SetSize(overlayPos_[0], overlayPos_[1]);
		dimming_->SettingWvp();
		dimming_.get()->SetBlend(BlendMode::kBlendModeNormal);
		isInitialized_ = true;


		int ChoicesImage[4];
		ChoicesImage[0] = texture.get()->CreateTexture("resources/white64x64.png");;
		ChoicesImage[1] = texture.get()->CreateTexture("resources/white64x64.png");;
		ChoicesImage[2] = texture.get()->CreateTexture("resources/white64x64.png");;
		ChoicesImage[3] = texture.get()->CreateTexture("resources/white64x64.png");;

		for (int i = 0; i < 4; i++) {
			Choices_[i] = std::make_unique<Sprite>();
			Choices_[i].get()->Initialize(ChoicesImage[i]);
			Vector2 ChoicesPos[2] = { {640.0f,100 + i * 64.0f}, {640.0f + 320.0f,100 + i * 64.0f} };
			Choices_[i].get()->SetSize(ChoicesPos[0], ChoicesPos[1]);
			Choices_[i]->SettingWvp();
			Choices_[i].get()->SetBlend(BlendMode::kBlendModeNormal);
		}

	}

	isPause_ = true;
	isSelect_ = false;
	isRestart_ = false;
	dimming_.get()->GetMatrial()->SetColor(dimmingColor_);
}

void Pause::Update() {

	switch (menuCommand_)
	{
	case kReturnToGame:
		if (Input::PushKey(DIK_S)) {
			menuCommand_ = kRestart;
		}
		if (Input::PushKey(DIK_SPACE)) {
			isPause_ = false;
		}
		break;
	case kRestart:
		if (Input::PushKey(DIK_W)) {
			menuCommand_ = kReturnToGame;
		}
		if (Input::PushKey(DIK_S)) {
			menuCommand_ = kReturnToStageSelect;
		}
		if (Input::PushKey(DIK_SPACE)) {
			isRestart_ = true;
		}
		break;
	case kReturnToStageSelect:
		if (Input::PushKey(DIK_W)) {
			menuCommand_ = kRestart;
		}
		if (Input::PushKey(DIK_S)) {
			menuCommand_ = kEnd;
		}
		if (Input::PushKey(DIK_SPACE)) {
			isSelect_ = true;
		}
		break;
	case kEnd:
		if (Input::PushKey(DIK_W)) {
			menuCommand_ = kReturnToStageSelect;
		}
		if (Input::PushKey(DIK_SPACE)) {
			Engine::SetEnd(true);
		}
		break;
	default:
		break;
	}

	for (int i = 0; i < 4; i++) {
		Choices_[i].get()->GetMatrial()->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		Choices_[i].get()->SettingWvp();
	}
	dimming_.get()->SettingWvp();

}

void Pause::Draw() {

	Draw::DrawSprite(dimming_.get());

	for (int i = 0; i < 4; i++) {
		Draw::DrawSprite(Choices_[i].get());
	}

}

