#pragma once  
#define DIRECTINPUT_VERSION 0x0800  
#include <dinput.h>  
#include <cstdint>  
#include "../Core/VariableTypes.h" 

#pragma comment(lib,"dinput8.lib")  
#pragma comment(lib,"dxguid.lib")  

class Input  
{  
private:  
	IDirectInput8* directInput_{};
	HRESULT result{};
	IDirectInputDevice8* keyboard{};
	
	BYTE key_[256] = {};  
	BYTE prevKey_[256] = {};  

	IDirectInputDevice8* mouse{};

	DIMOUSESTATE mouseState{};  
	DIMOUSESTATE prevMouseState{};  
	

public:  
	void Initialize(WNDCLASS wc, HWND hwnd);  
	void CreateInpuDevice();  
	void SetInputType();  
	void SetExclusionLevel(HWND hwnd);  
	void Updata();  

	//押して瞬間  
	bool PushKey(uint32_t Key);  
	//押している  
	bool PressKey(uint32_t key);  
	//離した瞬間  
	bool ReleaseKey(uint32_t key);  
	//離してる  
	bool FreeKey(uint32_t key);  

	//押して瞬間  
	bool PushMouse(uint32_t Key);  
	//押している  
	bool PressMouse(uint32_t key);  
	//離した瞬間  
	bool ReleaseMouse(uint32_t key);  
	//離してる  
	bool FreeMouse(uint32_t key);  
	//マウスの移動  
	Vector2i GetMouseDelta() { return Vector2i(mouseState.lX, mouseState.lY); }  
	int GetMouseWheel()const { return mouseState.lZ; }
	
	BYTE GetKey(int keyNum)const { return key_[keyNum]; }
};
