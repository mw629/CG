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
	{"3",MapChipType::kPlayerSpawn},
	{"4",MapChipType::kGoal},
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

	mapChipData.data.resize(kNumBlockVirtical_);
	for (auto& row : mapChipData.data) {
		row.resize(kNumBlockHorizontal_);
	}

	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	// マップチップCSV
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical_; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal_; ++j) {

			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData.data[i][j] = mapChipTable[word];
			}
		}
	}
	return mapChipData;
}

MapChipData MapManager::RandomMapData() {

	std::random_device rd;
	std::mt19937 mt(rd());

	std::vector<double> weights;
	for (const auto& p : mapList) {
		weights.push_back(p.weight);
	}

	// 驥阪∩莉倥″繝ｩ繝ｳ繝繝�驕ｸ謚�
	std::discrete_distribution<int> dist(weights.begin(), weights.end());
	int index = dist(mt);

	return MapData(index);
}

MapChipData MapManager::MapData(int index)
{
	return mapList[index].mapChipData;
}

