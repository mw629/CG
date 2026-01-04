#include "Pause.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif


void Pause::ImGui(){
#ifdef USE_IMGUI
    int selectInt = static_cast<int>(selectMode_);
    ImGui::DragInt("select", &selectInt, 1, backGame, gameEnd);
    selectMode_ = static_cast<SelectMode>(selectInt);
#endif
}

void Pause::Initialize() {
	isPause_ = true;
	isSelect_ = false;
	selectMode_ = backGame;

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int textureHndle = texture.get()->CreateTexture("resources/white64x64.png");
	dimming_.get()->Initialize(textureHndle);
	Vector2 overlayPos_[2] = { {0.0f,0.0f}, {1280.0f,720.0f} };
	dimming_.get()->SetSize(overlayPos_[0], overlayPos_[1]);
	dimming_->SettingWvp();
	dimming_.get()->SetBlend(BlendMode::kBlendModeNormal);
	dimming_.get()->GetMatrial()->SetColor(dimmingColor_);
}

void Pause::Update() {

	
		switch (selectMode_)
		{
		case Pause::backGame:
			if (Input::PushKey(DIK_S)) {
				selectMode_ = backSelectScene;
			}
			if (Input::PushKey(DIK_SPACE)) {
				isPause_ = false;
			}
			break;
		case Pause::backSelectScene:
			if (Input::PushKey(DIK_W)) {
				selectMode_ = backGame;
			}
			if (Input::PushKey(DIK_S)) {
				selectMode_ = gameEnd;
			}
			if (Input::PushKey(DIK_SPACE)) {
				isSelect_ = true;
			}
			break;
		case Pause::gameEnd:
			if (Input::PushKey(DIK_W)) {
				selectMode_ = backSelectScene;
			}
			if (Input::PushKey(DIK_SPACE)) {
				Engine::SetEnd(true);
			}
			break;
		default:
			break;
		}
	
		dimming_.get()->SettingWvp();

}

void Pause::Draw() {
	Draw::DrawSprite(dimming_.get());
}

