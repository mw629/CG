#pragma once
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include "../Core/VariableTypes.h"

class GamePadInput
{
private:
	XINPUT_STATE currentState_;
	XINPUT_STATE prevState_;
	int deadZone_ = 8000;
public:

	void DrawImGui();

	GamePadInput();

	void Update();

	/*
	XINPUT_GAMEPAD_A
	XINPUT_GAMEPAD_B
	XINPUT_GAMEPAD_X
	XINPUT_GAMEPAD_Y
	XINPUT_GAMEPAD_LEFT_SHOULDER
	XINPUT_GAMEPAD_RIGHT_SHOULDER
	XINPUT_GAMEPAD_BACK
	XINPUT_GAMEPAD_START
	XINPUT_GAMEPAD_DPAD_UP
	XINPUT_GAMEPAD_DPAD_DOWN
	XINPUT_GAMEPAD_DPAD_LEFT
	XINPUT_GAMEPAD_DPAD_RIGHT
	*/

	// ボタン入力チェック
	
	bool PressedButton(WORD button) const;
	bool TriggeredButton(WORD button) const;
	bool ReleasedButton(WORD button) const;
	bool FreeButton(WORD button)const;

	// スティックの値取得（-1.0f〜1.0f）
	
	Vector3 GetLeftStick()const { return { GetLeftStickX(),GetLeftStickY() ,0.0f }; }
	Vector3 GetRightStick()const { return { GetRightStickX(),GetRightStickY() ,0.0f }; }

	float GetLeftStickX() const;
	float GetLeftStickY() const;
	float GetRightStickX() const;
	float GetRightStickY() const;

};

