#pragma once

enum SceneID
{
	Test = 0,
	Title = 1,
	StageSelect = 2,
	Game = 3,
	Clear = 4,
	GameOver = 5
};

class IScene
{
protected:

	int sceneID_ = -1;
	int nextSceneID_;
	bool sceneChangeRequest_;

public:

	virtual ~IScene() {};

	virtual void ImGui() = 0;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	int GetSceneID() { return sceneID_; }
	int GetNextSceneID() { return nextSceneID_; }
	bool GetSceneChangeRequest() { return sceneChangeRequest_; }

};
