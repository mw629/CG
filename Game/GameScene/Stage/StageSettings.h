#pragma once


class StageSettings
{
private:

	//レーンの管理
	int minLaneIndex = 1;
	int maxLaneIndex = 1;
	float laneWidth = 1.0f;

public:

	void Initialize();
	void Update();
	void Draw();


	//ゲッター
	int GetMinLaneIndex() { return minLaneIndex; }
	int GetMaxLaneIndex() { return maxLaneIndex; }


};

