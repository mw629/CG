#include "Input.h"
#include <cassert>
#include <cstdint>
#include <cstring> 



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
}

void Input::SetInputType()
{
	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));
}

void Input::SetExclusionLevel(HWND hwnd)
{
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Updata()
{
	std::memcpy(prevKey_, key_, sizeof(key_));
	//kyeの取得
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(key_), key_);
}

bool Input::PushKey(uint32_t key)
{
	return (key_[key] & 0x80) && !(prevKey_[key] & 0x80);
}
bool Input::PressKey(uint32_t key) {
	return (key_[key] & 0x80);
}
bool Input::ReleaseKey(uint32_t key) {
	return !(key_[key] & 0x80) && (prevKey_[key] & 0x80);
}
bool Input::FreeKey(uint32_t key)
{
	return !(key_[key] & 0x80);
}







