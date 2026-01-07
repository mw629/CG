#pragma once

enum SceneID
{
	Title = 0,
	StageSelect = 1,
	Game = 2,
	Clear = 3,
	GameOver = 4
};

class IScene
{
protected:

	int sceneID_ = -1;
	int nextSceneID_;
	bool sceneChangeRequest_;

public:
	
	virtual ~IScene(){};

	virtual void ImGui() = 0;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	int GetSceneID() { return sceneID_; }
	int GetNextSceneID() { return nextSceneID_; }
	bool GetSceneChangeRequest() { return sceneChangeRequest_; }

};
