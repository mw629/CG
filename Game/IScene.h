#pragma once

enum SceneID
{
	kTitle = 0,
	kStageSelect = 1,
	kGame = 2,
	kClear = 3,
	kGameOver = 4
};

class IScene
{
protected:

	int sceneID_ = -1;
	int nextSceneID_;
	bool sceneChangeRequest_;

public:

	virtual ~IScene() {};

	virtual void ImGui() {}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Draw() {}

	int GetSceneID() { return sceneID_; }
	int GetNextSceneID() { return nextSceneID_; }
	bool GetSceneChangeRequest() { return sceneChangeRequest_; }

};
