#include "EditorManager.h"
#include <Engine.h>

#ifdef _USE_IMGUI
#include "../externals/imgui/imgui.h"
#endif // _USE_IMGUI

bool EditorManager::isPlaying_ = false;
EditorManager::SceneOverlayCallback EditorManager::s_sceneOverlayCallback_ = nullptr;
EditorManager::EditorCallback EditorManager::s_saveCallback_ = nullptr;
EditorManager::EditorCallback EditorManager::s_loadCallback_ = nullptr;

void EditorManager::Update(Engine* engine)
{
#ifdef _USE_IMGUI

	// ===== Unity風メインメニューバー =====
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
				if (s_saveCallback_) s_saveCallback_();
			}
			if (ImGui::MenuItem("Load", "Ctrl+L")) {
				if (s_loadCallback_) s_loadCallback_();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				Engine::SetEnd(true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			ImGui::MenuItem("Debug Info", nullptr, &showFinalWindow_);
			ImGui::EndMenu();
		}

		// --- メニューバー中央にPlay/Stopボタン ---
		float menuBarWidth = ImGui::GetWindowWidth();
		float buttonAreaWidth = 140.0f;
		ImGui::SetCursorPosX((menuBarWidth - buttonAreaWidth) * 0.5f);

		if (isPlaying_) {
			// Stopボタン（赤）
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.15f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.25f, 0.25f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
			if (ImGui::Button("  Stop  ")) {
				isPlaying_ = false;
			}
			ImGui::PopStyleColor(3);
		} else {
			// Playボタン（緑）
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.65f, 0.15f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.75f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.55f, 0.1f, 1.0f));
			if (ImGui::Button("  Play  ")) {
				isPlaying_ = true;
			}
			ImGui::PopStyleColor(3);
		}

		ImGui::EndMainMenuBar();
	}

	// Ctrl+S / Ctrl+L ショートカット
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
		if (s_saveCallback_) s_saveCallback_();
	}
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_L)) {
		if (s_loadCallback_) s_loadCallback_();
	}

	// ウィンドウ全体をDockSpaceとして設定（各ImGuiウィンドウをドッキング固定可能にする）
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	if (showFinalWindow_) {
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Debug Info", &showFinalWindow_);

		if (ImGui::BeginTabBar("DebugTabs")) {
			if (ImGui::BeginTabItem("System Data")) {
				ImGui::Text("--- Performance ---");
				ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

				static int frameCount = 0;
				frameCount++;
				ImGui::Text("Frame Count: %d", frameCount);

				size_t mem = engine->GetProcessMemoryUsage();
				ImGui::Text("Memory Usage: %.2f MB", mem / (1024.0f * 1024.0f));
				ImGui::Spacing();

				ImGui::Text("--- Application ---");
				ImGui::Text("Resolution: %d x %d", engine->GetClientWidth(), engine->GetClientHeight());
				ImGui::Spacing();

				ImGui::Text("--- Light Settings ---");
				static bool showLight = false;
				ImGui::Checkbox("Enable Light Settings (Shortcut: F1)", &showLight);
				if (ImGui::IsKeyPressed(ImGuiKey_F1))
				{
					showLight = !showLight;
				}
				
				if (showLight)
				{
					if (engine->GetLightManager()) {
						engine->GetLightManager()->ImGui();
					}
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Post Effect")) {
				if (engine->GetPostEffect()) {
					engine->GetPostEffect()->ImGuiWindow();
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Resource List")) {
				if (engine->GetTextureLoader()) {
					engine->GetTextureLoader()->Draw();
				}
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}

	ImGui::Begin("Scene");

	// --- アスペクト比設定 ---
	const char* aspectLabels[] = { "Free", "16:9", "4:3", "1:1", "21:9" };
	const float aspectRatios[] = { 0.0f, 16.0f / 9.0f, 4.0f / 3.0f, 1.0f, 21.0f / 9.0f };
	ImGui::Combo("Aspect Ratio", &sceneAspectRatioIndex_, aspectLabels, IM_ARRAYSIZE(aspectLabels));
	ImGui::Separator();

	ImVec2 availSize = ImGui::GetContentRegionAvail();
	ImVec2 imageSize = availSize;
	ImVec2 cursorStart = ImGui::GetCursorPos();

	if (sceneAspectRatioIndex_ > 0 && availSize.x > 0.0f && availSize.y > 0.0f) {
		float targetAspect = aspectRatios[sceneAspectRatioIndex_];
		float availAspect = availSize.x / availSize.y;

		if (availAspect > targetAspect) {
			// ウィンドウが横に広い → 高さに合わせ、左右に余白
			imageSize.y = availSize.y;
			imageSize.x = availSize.y * targetAspect;
		} else {
			// ウィンドウが縦に長い → 幅に合わせ、上下に余白
			imageSize.x = availSize.x;
			imageSize.y = availSize.x / targetAspect;
		}

		// 余白を計算して中央配置
		float offsetX = (availSize.x - imageSize.x) * 0.5f;
		float offsetY = (availSize.y - imageSize.y) * 0.5f;
		ImGui::SetCursorPos(ImVec2(cursorStart.x + offsetX, cursorStart.y + offsetY));
	}

	if (engine->GetRenderTexture()) {
		ImGui::Image((ImTextureID)engine->GetRenderTexture()->GetSrvHandleGPU().ptr, imageSize);
	}

	// ギズモ等のオーバーレイ描画コールバックをSceneウィンドウのBegin/Endの間に呼び出す
	if (s_sceneOverlayCallback_) {
		s_sceneOverlayCallback_();
	}
	ImGui::End();

#endif
}
