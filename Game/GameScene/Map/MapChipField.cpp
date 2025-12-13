#include "MapChipField.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

void MapChipField::SetMapData(MapChipData mapData)
{
	mapChipData_ = mapData;
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || mapChipData_.width - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || mapChipData_.height - 1 < yIndex) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return Vector3(kBlockWidth_ * xIndex, kBlockHeight_ * (mapChipData_.height - 1 - yIndex), 0);
}

IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth_ / 2.0f) / kBlockWidth_);
	indexSet.yIndex = static_cast<uint32_t>((position.y + kBlockHeight_ / 2.0f) / kBlockHeight_);
	indexSet.yIndex = mapChipData_.height - 1 - indexSet.yIndex;
	return indexSet;
}

Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	// 指定ブロックの中心座標を取得する
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);
	Rect rect{};
	rect.left = center.x - kBlockWidth_ / 2.0f;
	rect.right = center.x + kBlockWidth_ / 2.0f;
	rect.bottom = center.y - kBlockHeight_ / 2.0f;
	rect.top = center.y + kBlockHeight_ / 2.0f;
	return rect;
}

Vector3 MapChipField::GetPos(MapChipType mapChipType)
{
	for (uint32_t y = 0; y < mapChipData_.height; ++y) {
		for (uint32_t x = 0; x < mapChipData_.width; ++x) {
			if (mapChipData_.data[y][x] == mapChipType) {
				return GetMapChipPositionByIndex(x, y);
			}
		}
	}
	return Vector3(0, 0, 0);
}

std::list<Vector3> MapChipField::GetEnemyPosition(int i)
{
	std::list<Vector3> vector;
	if (i == 0) {
		for (uint32_t y = 0; y < mapChipData_.height; ++y) {
			for (uint32_t x = 0; x < mapChipData_.width; ++x) {
				if (mapChipData_.data[y][x] == MapChipType::kStopEnemySpawn) {
					vector.push_back(GetMapChipPositionByIndex(x, y));
				}
			}
		}
	}
	if (i == 1) {
		for (uint32_t y = 0; y < mapChipData_.height; ++y) {
			for (uint32_t x = 0; x < mapChipData_.width; ++x) {
				if (mapChipData_.data[y][x] == MapChipType::kRunEnemySpawn) {
					vector.push_back(GetMapChipPositionByIndex(x, y));
				}
			}
		}
	}
	if (i == 2) {
		for (uint32_t y = 0; y < mapChipData_.height; ++y) {
			for (uint32_t x = 0; x < mapChipData_.width; ++x) {
				if (mapChipData_.data[y][x] == MapChipType::kJumpEnemySpawn) {
					vector.push_back(GetMapChipPositionByIndex(x, y));
				}
			}
		}
	}
	return vector;
}
