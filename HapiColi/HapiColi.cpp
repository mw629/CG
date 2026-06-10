#include "HapiColi.h"
#include "HapiColi_Collision.h"
#include "HapiColi_Tester.h"
#include <imgui.h>


namespace HapiColi
{
	void HapiColi::ImGui()
	{
		ImGui::Begin("HapiColi");
		ImGui::Text("Hello, HapiColi!");
		ImGui::End();
	}

	void HapiColi::Initialize()
	{
		// 初期化の実装
	}

	void HapiColi::StartFrame()
	{
		// フレーム開始の実装
	}

	void HapiColi::EndFrame()
	{
		// フレーム終了の実装
	}

}