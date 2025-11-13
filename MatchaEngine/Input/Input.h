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

	IDirectInputDevice8* mouse{};

	

public:  
	void Initialize(WNDCLASS wc, HWND hwnd);  
	void CreateInpuDevice();  
	void SetInputType();  
	void SetExclusionLevel(HWND hwnd);  
	void Update();

	//押して瞬間  
	static bool PushKey(uint32_t Key);  
	//押している  
	static bool PressKey(uint32_t key);
	//離した瞬間  
	static bool ReleaseKey(uint32_t key);
	//離してる  
	static bool FreeKey(uint32_t key);

	//押して瞬間  
	static bool PushMouse(uint32_t Key);
	//押している  
	static bool PressMouse(uint32_t key);
	//離した瞬間  
	static bool ReleaseMouse(uint32_t key);
	//離してる  
	static bool FreeMouse(uint32_t key);

	//マウスの移動  
	static Vector2 GetMouseDelta();
	static int GetMouseWheel();
	


};
