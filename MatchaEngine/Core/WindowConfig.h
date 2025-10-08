#pragma once
#include "Windows.h"
#include "VariableTypes.h"
#include <cstdint>


class WindowConfig
{
private:

	WNDCLASS wc{};
	HWND hwnd{};
	
	Vector2Int ClientArea_;

public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void SetWindowData(const int32_t kClientWidth, const int32_t kClientHeight);

	void DrawWindow(const int32_t kClientWidth, const int32_t kClientHeight);

	void Finalize();

	static bool ProcessMassage();

	WNDCLASS GetWc()const { return wc; }
	HWND GetHwnd()const { return hwnd; }
};

