#include "GamePadInput.h"
#include <cmath>
#include <imgui.h>

namespace {
	XINPUT_STATE currentState;
	XINPUT_STATE prevState;
	int deadZone;
}

void GamePadInput::DrawImGui()
{
	bool press = PressButton(XINPUT_GAMEPAD_A);
	bool trigger = PushButton(XINPUT_GAMEPAD_A);
	bool releas = ReleaseButton(XINPUT_GAMEPAD_A);
	bool free = FreeButton(XINPUT_GAMEPAD_A);

	Vector3 leftStick = GetLeftStick();
	Vector3 rightStick = GetRightStick();

	ImGui::Begin("Contoller");
	ImGui::Checkbox("Press", &press);
	ImGui::Checkbox("Trigger", &trigger);
	ImGui::Checkbox("Releas", &releas);
	ImGui::Checkbox("Free", &free);


	ImGui::DragFloat3("LeftStick", &leftStick.x, 1.0f);
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
	SetPad();
}


///ボタンの入力///

bool GamePadInput::PushButton(WORD button)
{
	return (currentState.Gamepad.wButtons & button) &&
		!(prevState.Gamepad.wButtons & button);
}

bool GamePadInput::PressButton(WORD button)
{
	return (currentState.Gamepad.wButtons & button) != 0;
}

bool GamePadInput::ReleaseButton(WORD button)
{
	return !(currentState.Gamepad.wButtons & button) &&
		(prevState.Gamepad.wButtons & button);
}

bool GamePadInput::FreeButton(WORD button)
{
	return !(currentState.Gamepad.wButtons & button) &&
		!(prevState.Gamepad.wButtons & button);
}

///スティックの入力///

Vector3 GamePadInput::GetLeftStick()
{
	return { GetLeftStickX(),GetLeftStickY() ,0.0f };
}


Vector3 GamePadInput::GetRightStick()
{
	return { GetRightStickX(),GetRightStickY() ,0.0f };
}

float GamePadInput::GetLeftStickX()
{
	SHORT x = currentState.Gamepad.sThumbLX;
	return (std::abs(x) > deadZone) ? x / 32768.0f : 0.0f;//DeadZoneより小さいなら0.0fをかえす
}

float GamePadInput::GetLeftStickY()
{
	SHORT y = currentState.Gamepad.sThumbLY;
	return (std::abs(y) > deadZone) ? y / 32768.0f : 0.0f;
}

float GamePadInput::GetRightStickX() 
{
	SHORT x = currentState.Gamepad.sThumbRX;
	return (std::abs(x) > deadZone) ? x / 32768.0f : 0.0f;
}

float GamePadInput::GetRightStickY() 
{
	SHORT y = currentState.Gamepad.sThumbRY;
	return (std::abs(y) > deadZone) ? y / 32768.0f : 0.0f;
}

void GamePadInput::SetPad()
{
	currentState= currentState_;
	prevState= prevState_;
	deadZone= deadZone_;
}

