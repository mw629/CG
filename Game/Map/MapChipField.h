#pragma once
#include <Engine.h>  
#include "MapStruct.h"

class MapChipField {
private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth_ = 1.0f;
	static inline const float kBlockHeight_ = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical_ = 12;
	static inline const uint32_t kNumBlockHorizontal_ = 20;
	MapChipData mapChipData_;
public:
	static float GetBlockWidth() { return kBlockWidth_; };
	static float GetBlockHeight() { return kBlockHeight_; };

	uint32_t GetNumBlockVirtical() { return kNumBlockVirtical_; };
	uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal_; };

	void SetMapData(MapChipData mapData);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetPos(MapChipType mapChipType);

	Vector3 GetPlayerSpawn() { return GetPos(MapChipType::kPlayerSpawn); };
	Vector3 GetGoalPosition() { return GetPos(MapChipType::kGoal); };


};
