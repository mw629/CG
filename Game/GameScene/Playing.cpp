#include "Playing.h"
#include "Map/MapManager.h"




void Playing::ImGui() {

}

void Playing::Initialize(Matrix4x4 viewMatrix) {

	mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_.get()->SetMapData(MapManager::MapData(0));//stage数を引数に入れる

	mapchip_ = std::make_unique<Block>();
	mapchip_.get()->Initialize(mapChipField_.get(), viewMatrix);

	player_ = std::make_unique<Player>();
	player_.get()->Initialize(mapChipField_.get()->GetPlayerSpawn(), viewMatrix);
	player_.get()->SetMapChipField(mapChipField_.get());


}

void Playing::Update(Matrix4x4 viewMatrix) {

	player_.get()->Update(viewMatrix);

	mapchip_.get()->Update(viewMatrix);

}
void Playing::Draw() {

	player_.get()->Draw();
	mapchip_.get()->Draw();
}
