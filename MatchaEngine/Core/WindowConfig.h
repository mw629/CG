#include "Windows.h"
#include <cstdint>

#pragma once

class WindowConfig
{
private:

	WNDCLASS wc{};
	HWND hwnd{};

public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void SetWindowData(const int32_t kClientWidth, const int32_t kClientHeight);

	void DrawWindow(const int32_t kClientWidth, const int32_t kClientHeight);

	WNDCLASS GetWc()const { return wc; }
	HWND GetHwnd()const { return hwnd; }
};

