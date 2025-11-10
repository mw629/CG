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
	ZeroMemory(&currentState, sizeof(XINPUT_STATE));
	ZeroMemory(&prevState, sizeof(XINPUT_STATE));
}

void GamePadInput::Update()
{
	prevState = currentState;
	ZeroMemory(&currentState, sizeof(XINPUT_STATE));
	XInputGetState(0, &currentState); // プレイヤー1のコントローラー
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

void GamePadInput::SetVibration(WORD leftMotorSpeed, WORD rightMotorSpeed)
{
	{
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

		// 左モーター（低周波）の速度を設定
		// 0: 振動なし, 65535: 最大振動
		vibration.wLeftMotorSpeed = leftMotorSpeed;

		// 右モーター（高周波）の速度を設定
		// 0: 振動なし, 65535: 最大振動
		vibration.wRightMotorSpeed = rightMotorSpeed;

		// プレイヤー1 (dwUserIndex=0) に振動状態を設定
		// XInputSetStateが振動を設定するための関数
		XInputSetState(0, &vibration);
	}
}