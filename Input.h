#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <cstdint>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class Input
{
private:
	IDirectInput8* directInput_;
	HRESULT result;

	IDirectInputDevice8* keyboard; 

	
	BYTE key_[256] = {};

	BYTE prevKey_[256] = {};

public:
	void Initialize(WNDCLASS wc, HWND hwnd);

	void CreateInpuDevice();
	
	void SetInputType();

	void SetExclusionLevel(HWND hwnd);

	void Updata();


	bool PushKey(uint32_t Key);//押して瞬間
	bool PressKey(uint32_t key);//押している
	bool ReleaseKey(uint32_t key);//離した瞬間
	bool FreeKey(uint32_t key);//離してる
	

	BYTE GetKey(int keyNum) { return key_[keyNum]; }

};

