#include "GamePadInput.h"
#include <cmath>
#include <imgui.h>

void GamePadInput::DrawImGui()
{
	bool press = PressedButton(XINPUT_GAMEPAD_A);
	bool trigger = TriggeredButton(XINPUT_GAMEPAD_A);
	bool releas = ReleasedButton(XINPUT_GAMEPAD_A);
	bool free = FreeButton(XINPUT_GAMEPAD_A);

	Vector3 leftStick=GetLeftStick();
	Vector3 rightStick = GetRightStick();

	ImGui::Begin("Contoller");
	ImGui::Checkbox("Press", &press);
	ImGui::Checkbox("Trigger", &trigger);
	ImGui::Checkbox("Releas", &releas);
	ImGui::Checkbox("Free", &free);


	ImGui::DragFloat3("LeftStick", &leftStick.x,1.0f);
	ImGui::DragFloat3("RightStick", &rightStick.x, 1.0f);
	ImGui::End();

}

GamePadInput::GamePadInput()
{
	ZeroMemory(&currentState_, sizeof(XINPUT_STATE));
	ZeroMemory(&prevState_, sizeof(XINPUT_STATE));
}

void GamePadInput::Update()
{
	prevState_ = currentState_;
	ZeroMemory(&currentState_, sizeof(XINPUT_STATE));
	XInputGetState(0, &currentState_); // プレイヤー1のコントローラー
}


///ボタンの入力///

bool GamePadInput::PressedButton(WORD button) const
{
	return (currentState_.Gamepad.wButtons & button) != 0;
}

bool GamePadInput::TriggeredButton(WORD button) const
{
	return (currentState_.Gamepad.wButtons & button) &&
		!(prevState_.Gamepad.wButtons & button);
}

bool GamePadInput::ReleasedButton(WORD button) const
{
	return !(currentState_.Gamepad.wButtons & button) &&
		(prevState_.Gamepad.wButtons & button);
}

bool GamePadInput::FreeButton(WORD button) const
{
	return !(currentState_.Gamepad.wButtons & button)&&
		!(prevState_.Gamepad.wButtons & button);
}

///スティックの入力///

float GamePadInput::GetLeftStickX() const
{
	SHORT x = currentState_.Gamepad.sThumbLX;
	return (std::abs(x) > deadZone_) ? x / 32768.0f : 0.0f;//DeadZoneより小さいなら0.0fをかえす
}

float GamePadInput::GetLeftStickY() const
{
	SHORT y = currentState_.Gamepad.sThumbLY;
	return (std::abs(y) > deadZone_) ? y / 32768.0f : 0.0f;
}

float GamePadInput::GetRightStickX() const
{
	SHORT x = currentState_.Gamepad.sThumbRX;
	return (std::abs(x) > deadZone_) ? x / 32768.0f : 0.0f;
}

float GamePadInput::GetRightStickY() const
{
	SHORT y = currentState_.Gamepad.sThumbRY;
	return (std::abs(y) > deadZone_) ? y / 32768.0f : 0.0f;
}

