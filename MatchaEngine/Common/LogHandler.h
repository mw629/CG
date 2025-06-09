#pragma once

#include <format>
#include <string>
#include <filesystem>
#include <dxgidebug.h>


//現在時刻を取得する
std::ofstream CurrentTimestamp();

//Stringに変換
std::string ConvertString(const std::wstring& str);

std::wstring ConvertString(const std::string& str);

//ログを出す
void Log(std::ostream& os, const std::string& message);