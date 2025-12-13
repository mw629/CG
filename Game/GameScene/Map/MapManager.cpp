#include "MapManager.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

#include <filesystem>
#include <random>
namespace fs = std::filesystem;

namespace {
	std::vector<MapChipList> mapList;
	int mapNum;

	std::map<std::string, MapChipType> mapChipTable = {
	{"0",MapChipType::kBlank},
	{"1",MapChipType::kBlock},
	{"2",MapChipType::kGoal},
	{"3",MapChipType::kPlayerSpawn},
	{"4",MapChipType::kStopEnemySpawn},//敵のスポーン位置
	{"5", MapChipType::kRunEnemySpawn },
	{"6", MapChipType::kJumpEnemySpawn },
	{ "7", MapChipType::kNone },
	};
}


MapManager::MapManager()
{
	Initialize();
}

MapManager::~MapManager()
{

}

void MapManager::Initialize() {

	mapchipList_.clear();

	std::string basePath = "resources/MapData/";
	int index = 0;

	while (true) {
		std::string path = basePath + "Stage" + std::to_string(index) + ".csv";

		if (!fs::exists(path)) {
			break;
		}

		MapChipList mapchipList;

		mapchipList.parh = path;
		mapchipList.mapChipData = LoadMapChipCsv(path);
		mapchipList.weight = 1.0f;


		mapchipList_.push_back(mapchipList);

		++index;
	}
	mapList = mapchipList_;
	mapNum = index;

}

MapChipData MapManager::LoadMapChipCsv(const std::string& filePath) {

	MapChipData mapChipData;

	std::ifstream file(filePath);
	assert(file.is_open());

	std::vector<std::vector<MapChipType>> tempData;

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue; // 空行スキップ

		std::istringstream lineStream(line);
		std::vector<MapChipType> row;

		std::string word;
		while (std::getline(lineStream, word, ',')) {
			if (mapChipTable.contains(word)) {
				row.push_back(mapChipTable[word]);
			}
			else {
				// 未定義のワードが来たときの保険（空マス扱いなど）
				row.push_back(MapChipType::kNone);
			}
		}

		tempData.push_back(row);
	}

	file.close();

	// サイズに合わせてMapChipDataに代入
	mapChipData.data = std::move(tempData);

	mapChipData.width = mapChipData.data[0].size();
	mapChipData.height = mapChipData.data.size();

	return mapChipData;
}



MapChipData MapManager::MapData(int index)
{
	return mapList[index].mapChipData;
}

