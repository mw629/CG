#pragma once
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

#include "../Core/VariableTypes.h"

class GamePadInput
{
private:
	
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
	
	static bool PushButton(WORD button);
	static bool PressButton(WORD button);
	static bool ReleaseButton(WORD button);
	static bool FreeButton(WORD button);

	// スティックの値取得（-1.0f〜1.0f）
	
	static Vector3 GetLeftStick();  
	static Vector3 GetRightStick(); 

	static float GetLeftStickX();
	static float GetLeftStickY();
	static float GetRightStickX();
	static float GetRightStickY();

	// コントローラーを振動させる（0〜65535）
	static void SetVibration(WORD leftMotorSpeed, WORD rightMotorSpeed);


};

