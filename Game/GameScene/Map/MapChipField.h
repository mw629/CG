#pragma once
#include <Engine.h>  
#include "MapStruct.h"

class MapChipField {
private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth_ = 1.0f;
	static inline const float kBlockHeight_ = 1.0f;
	// ブロックの個数
	MapChipData mapChipData_;
public:
	static float GetBlockWidth() { return kBlockWidth_; };
	static float GetBlockHeight() { return kBlockHeight_; };

	uint32_t GetNumBlockVirtical() { return mapChipData_.height; };
	uint32_t GetNumBlockHorizontal() { return mapChipData_.width; };

	void SetMapData(MapChipData mapData);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetPos(MapChipType mapChipType);

	std::list<Vector3> GetEnemyPosition(int i);
	std::list<Vector3> GetBOSSPosition();
	Vector3 GetPlayerSpawn() { return GetPos(MapChipType::kPlayerSpawn); };
	Vector3 GetGoalPosition() { return GetPos(MapChipType::kGoal); };


};
