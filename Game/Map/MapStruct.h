#pragma once
#include <vector>
#include <map>
#include <string>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kPlayerSpawn,//プレイヤースポーン地点
	kGoal,//ゴール
};


struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
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
