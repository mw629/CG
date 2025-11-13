#pragma once
#include <string>
#include <Engine.h>
#include "MapStruct.h"


struct MapChipList {
	MapChipData mapChipData;
	double weight;
	std::string parh;

};

class MapManager
{

private:


	static inline const uint32_t kNumBlockVirtical_ = 12;
	static inline const uint32_t kNumBlockHorizontal_ = 20;

	std::vector<MapChipList> mapchipList_;

	


public:

	MapManager();
	~MapManager();

	void Initialize();

	MapChipData LoadMapChipCsv(const std::string& filePath);

	static MapChipData RandomMapData();

	static MapChipData MapData(int index);


};

