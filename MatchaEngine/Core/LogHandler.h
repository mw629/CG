#pragma once
#include <format>
#include <string>
#include <filesystem>
#include <dxgidebug.h>

#include <dbgHelp.h>
#include <strsafe.h>

#pragma comment(lib,"dbgHelp.lib")


//現在時刻を取得する
std::ofstream CurrentTimestamp();

//Stringに変換
std::string ConvertString(const std::wstring& str);

std::wstring ConvertString(const std::string& str);

#include <vector>

//ログを出す
void Log(std::ostream& os, const std::string& message);

// ログの取得とクリア用
const std::vector<std::string>& GetLogs();
void ClearLogs();

//CrashHandlerの登録//
LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);