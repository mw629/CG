#pragma once
#include <functional>
#include <memory>
#include <string>

class Engine;

class EditorManager {
public:
	// Sceneウィンドウ内のオーバーレイ描画用コールバック型
	using SceneOverlayCallback = std::function<void()>;
	// Save/Loadコールバック型
	using EditorCallback = std::function<void(const std::string&)>;

private:
	static bool isPlaying_;
	static SceneOverlayCallback s_sceneOverlayCallback_;
	static EditorCallback s_saveCallback_;
	static EditorCallback s_loadCallback_;
	static std::string s_currentFileName_;

	bool showFinalWindow_ = true;
	int sceneAspectRatioIndex_ = 0; // 0: Free, 1: 16:9, 2: 4:3, 3: 1:1, 4: 21:9

public:
	static void SetPlaying(bool playing) { isPlaying_ = playing; }
	static bool IsPlaying() { return isPlaying_; }

	static void SetSceneOverlayCallback(SceneOverlayCallback cb) { s_sceneOverlayCallback_ = cb; }
	static void ClearSceneOverlayCallback() { s_sceneOverlayCallback_ = nullptr; }

	static void SetSaveCallback(EditorCallback cb) { s_saveCallback_ = cb; }
	static void SetLoadCallback(EditorCallback cb) { s_loadCallback_ = cb; }

	// 毎フレーム呼ばれるエディタUIの更新処理
	void Update(Engine* engine);
};
