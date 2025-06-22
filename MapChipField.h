#pragma once
#include <vector>
#include "MatchaEngine/Common/VariableTypes.h"


enum class MapChipType {
	kBlank,//空白
	kBlock,//ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

struct Rect {
	float left;
	float right;
	float bottom;
	float top;
};

class MapChipField {

private:

	MapChipData mapChipData_;

	//1ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;
	//ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

public:

	int map[10][50] = {};

	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);

};
