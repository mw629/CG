#include "EditorManager.h"
#include <Engine.h>
#include <filesystem>
#include <vector>

#ifdef _USE_IMGUI
#include "../externals/imgui/imgui.h"
#endif // _USE_IMGUI

#include "../MatchaEngine/Core/LogHandler.h"
#include "../MatchaEngine/Resource/Texture.h"
#include <unordered_map>
#include <algorithm>

#ifdef _USE_IMGUI
static std::filesystem::path s_selectedResourceDir = "resources";
static std::unordered_map<std::string, D3D12_GPU_DESCRIPTOR_HANDLE> s_iconCache;
static std::unique_ptr<Texture> s_editorTexture;

void DrawDirectoryTree(const std::filesystem::path& dirPath) {
	try {
		for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
			if (entry.is_directory()) {
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				if (s_selectedResourceDir == entry.path()) {
					flags |= ImGuiTreeNodeFlags_Selected;
				}
				
				auto u8name = entry.path().filename().u8string();
				bool isOpen = ImGui::TreeNodeEx(reinterpret_cast<const char*>(u8name.c_str()), flags);
				if (ImGui::IsItemClicked()) {
					s_selectedResourceDir = entry.path();
				}
				if (isOpen) {
					DrawDirectoryTree(entry.path());
					ImGui::TreePop();
				}
			}
		}
	} catch (...) {}
}

void DrawDirectoryContents(const std::filesystem::path& dirPath) {
	try {
		float padding = 16.0f;
		float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
			std::string name;
			auto u8name = entry.path().filename().u8string();
			name = std::string(reinterpret_cast<const char*>(u8name.c_str()));
			
			bool isDirectory = entry.is_directory();
			std::string ext = entry.path().extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
			
			ImGui::PushID(name.c_str());
			
			ImTextureID texID = 0;
			bool isImage = false;
			
			if (!isDirectory && (ext == ".png" || ext == ".jpg" || ext == ".jpeg")) {
				isImage = true;
				auto u8str = entry.path().u8string();
				std::string pathStr(reinterpret_cast<const char*>(u8str.c_str()));
				std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

				if (s_iconCache.find(pathStr) == s_iconCache.end()) {
					if (!s_editorTexture) s_editorTexture = std::make_unique<Texture>();
					int id = s_editorTexture->CreateTexture(pathStr);
					s_iconCache[pathStr] = s_editorTexture->TextureData(id);
				}
				D3D12_GPU_DESCRIPTOR_HANDLE handle = s_iconCache[pathStr];
				texID = (ImTextureID)handle.ptr;
			}
			
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			if (isImage) {
				ImGui::ImageButton(name.c_str(), texID, ImVec2(thumbnailSize, thumbnailSize));
			} else {
				ImGui::Button("##Icon", ImVec2(thumbnailSize, thumbnailSize));
				
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 rectMin = ImGui::GetItemRectMin();
				ImVec2 rectMax = ImGui::GetItemRectMax();
				float itemW = rectMax.x - rectMin.x;
				float itemH = rectMax.y - rectMin.y;
				
				ImU32 outlineColor = IM_COL32(150, 150, 150, 255);
				if (ImGui::IsItemHovered()) outlineColor = IM_COL32(220, 220, 220, 255);
				
				if (isDirectory) {
					ImU32 folderColor = IM_COL32(234, 194, 82, 255);
					if (ImGui::IsItemHovered()) folderColor = IM_COL32(255, 214, 102, 255);
					ImU32 folderDark = IM_COL32(204, 164, 52, 255);
					float tabW = itemW * 0.45f;
					float tabH = itemH * 0.15f;
					drawList->AddRectFilled(rectMin, ImVec2(rectMin.x + tabW, rectMin.y + tabH), folderDark, 2.0f);
					drawList->AddRectFilled(ImVec2(rectMin.x, rectMin.y + tabH * 0.8f), ImVec2(rectMin.x + itemW, rectMin.y + itemH), folderColor, 4.0f);
				} else {
					ImU32 docColor = IM_COL32(230, 230, 230, 255);
					if (ImGui::IsItemHovered()) docColor = IM_COL32(250, 250, 250, 255);
					
					ImU32 labelBgColor = IM_COL32(100, 100, 100, 255);
					if (ext == ".json") labelBgColor = IM_COL32(180, 180, 50, 255);
					else if (ext == ".dds") labelBgColor = IM_COL32(150, 50, 50, 255);
					else if (ext == ".obj" || ext == ".gltf") labelBgColor = IM_COL32(50, 150, 200, 255);
					else if (ext == ".hlsl" || ext == ".hlsli") labelBgColor = IM_COL32(50, 200, 150, 255);
					
					float foldSize = itemW * 0.25f;
					
					drawList->AddRectFilled(rectMin, ImVec2(rectMin.x + itemW, rectMin.y + itemH), docColor, 2.0f);
					drawList->AddRect(rectMin, ImVec2(rectMin.x + itemW, rectMin.y + itemH), outlineColor, 2.0f);
					
					ImVec2 foldPts[3] = {
						ImVec2(rectMin.x + itemW - foldSize, rectMin.y),
						ImVec2(rectMin.x + itemW - foldSize, rectMin.y + foldSize),
						ImVec2(rectMin.x + itemW, rectMin.y + foldSize)
					};
					drawList->AddConvexPolyFilled(foldPts, 3, IM_COL32(180, 180, 180, 255));
					
					float labelH = itemH * 0.35f;
					ImVec2 labelPos = ImVec2(rectMin.x, rectMin.y + itemH * 0.45f);
					drawList->AddRectFilled(labelPos, ImVec2(rectMin.x + itemW, labelPos.y + labelH), labelBgColor);
					
					std::string text = ext.empty() ? "" : ext.substr(1);
					std::transform(text.begin(), text.end(), text.begin(), ::toupper);
					
					if (text.length() > 4) text = text.substr(0, 4); // Keep text short
					
					ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
					ImVec2 textPos = ImVec2(rectMin.x + (itemW - textSize.x) * 0.5f, labelPos.y + (labelH - textSize.y) * 0.5f);
					drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), text.c_str());
				}
			}
			ImGui::PopStyleColor();
			
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (isDirectory) {
					s_selectedResourceDir = entry.path();
				}
			}
			
			ImGui::TextWrapped("%s", name.c_str());
			
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
	} catch (...) {}
}
#endif // _USE_IMGUI

#ifdef _USE_IMGUI
bool EditorManager::isPlaying_ = false;
#else
bool EditorManager::isPlaying_ = true;
#endif
EditorManager::SceneOverlayCallback EditorManager::s_sceneOverlayCallback_ = nullptr;
EditorManager::EditorCallback EditorManager::s_saveCallback_ = nullptr;
EditorManager::EditorCallback EditorManager::s_loadCallback_ = nullptr;
std::string EditorManager::s_currentFileName_ = "scene";

void EditorManager::Update(Engine* engine)
{
#ifdef _USE_IMGUI

	static bool showSaveAsPopup = false;
	static bool showLoadPopup = false;
	static char fileNameBuffer[256] = "";

	auto getFullPath = [](const std::string& name) {
		std::string fname = name;
		if (fname.length() < 5 || fname.substr(fname.length() - 5) != ".json") {
			fname += ".json";
		}
		return "Resources/Json/Scene/" + fname;
	};

	// ===== Unity風メインメニューバー =====
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			std::string displayFileName = s_currentFileName_.empty() ? "None" : s_currentFileName_ + ".json";
			ImGui::TextDisabled("Current File: %s", displayFileName.c_str());
			ImGui::Separator();

			if (ImGui::MenuItem("Save", "Ctrl+S")) {
				if (s_currentFileName_.empty()) {
					showSaveAsPopup = true;
					snprintf(fileNameBuffer, sizeof(fileNameBuffer), "%s", "scene");
				} else {
					if (s_saveCallback_) s_saveCallback_(getFullPath(s_currentFileName_));
				}
			}
			if (ImGui::MenuItem("Save As...")) {
				showSaveAsPopup = true;
				snprintf(fileNameBuffer, sizeof(fileNameBuffer), "%s", s_currentFileName_.empty() ? "scene" : s_currentFileName_.c_str());
			}
			if (ImGui::MenuItem("Load...", "Ctrl+L")) {
				showLoadPopup = true;
				snprintf(fileNameBuffer, sizeof(fileNameBuffer), "%s", s_currentFileName_.empty() ? "scene" : s_currentFileName_.c_str());
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				Engine::SetEnd(true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window")) {
			ImGui::MenuItem("Debug Info", nullptr, &showFinalWindow_);
			ImGui::MenuItem("Resources", nullptr, &showResourcesWindow_);
			ImGui::MenuItem("Logs", nullptr, &showLogsWindow_);
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
		if (s_currentFileName_.empty()) {
			showSaveAsPopup = true;
			snprintf(fileNameBuffer, sizeof(fileNameBuffer), "%s", "scene");
		} else {
			if (s_saveCallback_) s_saveCallback_(getFullPath(s_currentFileName_));
		}
	}
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_L)) {
		showLoadPopup = true;
		snprintf(fileNameBuffer, sizeof(fileNameBuffer), "%s", s_currentFileName_.empty() ? "scene" : s_currentFileName_.c_str());
	}

	// Save As... ポップアップ
	if (showSaveAsPopup) {
		ImGui::OpenPopup("Save As...");
		showSaveAsPopup = false;
	}
	if (ImGui::BeginPopupModal("Save As...", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		std::string displayFileName = s_currentFileName_.empty() ? "None" : s_currentFileName_ + ".json";
		ImGui::TextDisabled("Current File: %s", displayFileName.c_str());
		ImGui::Text("File name (saved in Resources/Json/Scene/):");
		ImGui::InputText("##savepath", fileNameBuffer, sizeof(fileNameBuffer));
		if (ImGui::Button("Save", ImVec2(120, 0))) {
			s_currentFileName_ = fileNameBuffer;
			if (s_saveCallback_) s_saveCallback_(getFullPath(s_currentFileName_));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	// Load Scene... ポップアップ
	if (showLoadPopup) {
		ImGui::OpenPopup("Load Scene...");
		showLoadPopup = false;
	}
	if (ImGui::BeginPopupModal("Load Scene...", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		std::string displayFileName = s_currentFileName_.empty() ? "None" : s_currentFileName_ + ".json";
		ImGui::TextDisabled("Current File: %s", displayFileName.c_str());
		ImGui::Text("Select file to load from Resources/Json/Scene/:");

		// List files
		std::vector<std::string> jsonFiles;
		try {
			for (const auto& entry : std::filesystem::directory_iterator("Resources/Json/Scene")) {
				if (entry.is_regular_file() && entry.path().extension() == ".json") {
					jsonFiles.push_back(entry.path().stem().string());
				}
			}
		} catch (...) {}

		if (ImGui::BeginCombo("Target File", s_currentFileName_.c_str())) {
			for (const auto& file : jsonFiles) {
				bool is_selected = (s_currentFileName_ == file);
				if (ImGui::Selectable(file.c_str(), is_selected)) {
					s_currentFileName_ = file;
					snprintf(fileNameBuffer, sizeof(fileNameBuffer), "%s", file.c_str());
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("##loadpath", fileNameBuffer, sizeof(fileNameBuffer));

		if (ImGui::Button("Load", ImVec2(120, 0))) {
			s_currentFileName_ = fileNameBuffer;
			if (s_loadCallback_) s_loadCallback_(getFullPath(s_currentFileName_));
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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

	// Resources Window
	if (showResourcesWindow_) {
		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Resources", &showResourcesWindow_)) {
			if (ImGui::BeginTable("ResourceBrowser", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable)) {
				ImGui::TableNextRow();
				
				// Left pane: Directory tree
				ImGui::TableSetColumnIndex(0);
				ImGui::BeginChild("DirTree", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
				ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
				if (s_selectedResourceDir == "resources") rootFlags |= ImGuiTreeNodeFlags_Selected;
				bool rootOpen = ImGui::TreeNodeEx("resources", rootFlags);
				if (ImGui::IsItemClicked()) s_selectedResourceDir = "resources";
				if (rootOpen) {
					DrawDirectoryTree("resources");
					ImGui::TreePop();
				}
				ImGui::EndChild();

				// Right pane: Contents of selected directory
				ImGui::TableSetColumnIndex(1);
				ImGui::BeginChild("DirContents", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
				if (std::filesystem::exists(s_selectedResourceDir) && std::filesystem::is_directory(s_selectedResourceDir)) {
					ImGui::TextUnformatted(s_selectedResourceDir.string().c_str());
					ImGui::Separator();
					DrawDirectoryContents(s_selectedResourceDir);
				} else {
					ImGui::Text("Directory not found.");
				}
				ImGui::EndChild();

				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

	// Logs Window
	if (showLogsWindow_) {
		ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Logs", &showLogsWindow_)) {
			if (ImGui::Button("Clear Logs")) {
				ClearLogs();
			}
			ImGui::Separator();
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
			const auto& logs = GetLogs();
			for (const auto& log : logs) {
				ImGui::TextUnformatted(log.c_str());
			}
			// 自動スクロール
			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
				ImGui::SetScrollHereY(1.0f);
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

#endif
}
