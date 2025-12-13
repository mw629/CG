#pragma once
#include <vector>
#include <map>
#include <string>

enum class MapChipType {
	kBlank = 0, // 空白
	kBlock = 1, // ブロック
	kGoal = 2,//ゴール
	kPlayerSpawn = 3,//プレイヤースポーン地点
	kStopEnemySpawn = 4,//敵のスポーン位置
	kRunEnemySpawn = 5,//敵のスポーン位置
	kJumpEnemySpawn = 6,//敵のスポーン位置
	kNone,
};


struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
	float width;
	float height;
};

struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};
// 範囲矩形
struct Rect {
	float left;   // 左端
	float right;  // 右端
	float bottom; // 下端
	float top;    // 上端
};
