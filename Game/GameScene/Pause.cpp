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
	


}

void Pause::Draw() {

}

