#include "Input.h"
#include <cassert>
#include <cstdint>
#include <cstring> 

namespace {
	BYTE g_key[256];
	BYTE g_prevKey[256];

	DIMOUSESTATE g_mouseState{};
	DIMOUSESTATE g_prevMouseState{};
}

void Input::Initialize(WNDCLASS wc, HWND hwnd) {
	directInput_=nullptr;
	result = DirectInput8Create(
		wc.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));
	CreateInpuDevice();
	SetInputType();
	SetExclusionLevel(hwnd);
}

void Input::CreateInpuDevice()
{
	keyboard = nullptr;
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	mouse = nullptr;
	result = directInput_->CreateDevice(GUID_SysMouse, &mouse, NULL);
	assert(SUCCEEDED(result));
}

void Input::SetInputType()
{
	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));
	result = mouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
}

void Input::SetExclusionLevel(HWND hwnd)
{
	DWORD flags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY;
	result = keyboard->SetCooperativeLevel(hwnd, flags);
	assert(SUCCEEDED(result));
	result = mouse->SetCooperativeLevel(hwnd, flags);
	assert(SUCCEEDED(result));
}

void Input::Updata()
{
	//kyeの取得
	std::memcpy(prevKey_, key_, sizeof(key_));
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(key_), key_);
	//マウスの取得
	std::memcpy(&prevMouseState,&mouseState, sizeof(mouseState));
	mouse->Acquire();
	mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	SetKey();
}

bool Input::PushKey(uint32_t key)
{
	return (g_key[key] & 0x80) && !(g_prevKey[key] & 0x80);
}
bool Input::PressKey(uint32_t key) {
	return (g_key[key] & 0x80);
}
bool Input::ReleaseKey(uint32_t key) {
	return !(g_key[key] & 0x80) && (g_prevKey[key] & 0x80);
}
bool Input::FreeKey(uint32_t key)
{
	return !(g_key[key] & 0x80);
}

bool Input::PushMouse(uint32_t bottom)
{
	return (g_mouseState.rgbButtons[bottom] & 0x80) && !(g_prevMouseState.rgbButtons[bottom] & 0x80);
}
bool Input::PressMouse(uint32_t bottom) {
	return (g_mouseState.rgbButtons[bottom] & 0x80);
}
bool Input::ReleaseMouse(uint32_t bottom) {
	return !(g_mouseState.rgbButtons[bottom] & 0x80) && (g_prevMouseState.rgbButtons[bottom] & 0x80);
}
bool Input::FreeMouse(uint32_t bottom)
{
	return !(g_mouseState.rgbButtons[bottom] & 0x80);
}

Vector2i Input::GetMouseDelta()
{
	 return Vector2i(g_mouseState.lX, g_mouseState.lY); 
}

int Input::GetMouseWheel()
{
	 return g_mouseState.lZ; 
}

void Input::SetKey()
{
	for (int i = 0; i < 256; i++) {
		g_key[i] = key_[i];
		g_prevKey[i] = prevKey_[i];
	}
	g_mouseState= mouseState;
	g_prevMouseState= prevMouseState;
}









