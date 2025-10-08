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
	int nextScene_;
	bool sceneChangeRequest_;

public:

	virtual void ImGui() {}

	virtual void Initialize() {}

	virtual void Update(){}

	virtual void Draw(){}

	int GetSceneID() { return sceneID_; }
	int GetNextScene() { return nextScene_; }
	bool GetSceneChangeRequest() { return sceneChangeRequest_; }

};
