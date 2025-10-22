#include "Block.h"
#include "../../Map/MapChipField.h"


Block::Block()
{
}

Block::~Block()
{
	for (int i = 0; i < models_.size(); i++) {
		for (int j = 0; j < models_[i].size(); j++) {
			if (models_[i][j]) {
				delete models_[i][j];
			}
		}
	}
}



void Block::Initialize(MapChipField* mapChipField, Matrix4x4 viewMatrix) {

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	SetMapChipField(mapChipField);

	ModelData blockModel = LoadObjFile("resources/block", "block.obj");


	// 要素数
	uint32_t numBlockVirtocal = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	// 要素数を変更する

	for (int i = 0; i < numBlockVirtocal; i++) {
		std::vector<Model*> modelRow;
		for (int j = 0; j < numBlockHorizontal; j++) {
			Transform transform = {
				{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{1.0f + blockSize_ * j,1.0f + blockSize_ * i,0.0f } };
			transform.translate = mapChipField_->GetMapChipPositionByIndex(j, i);
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				Model* model = new Model();
				model->Initialize(blockModel);
				model->CreateModel();
				model->SetTransform(transform);
				model->SettingWvp(viewMatrix);
				modelRow.push_back(model);
			}
		}
		models_.push_back(modelRow);
	}

}


void Block::Update(Matrix4x4 viewMatrix) {

	for (int i = 0; i < models_.size(); i++) {
		for (int j = 0; j < models_[i].size(); j++) {
			if (models_[i][j]) {
				models_[i][j]->SettingWvp(viewMatrix);
			}
		}

	}

}

void Block::Draw() {
	for (int i = 0; i < models_.size(); i++) {
		for (int j = 0; j < models_[i].size(); j++) {
			if (models_[i][j]) {
				Draw::DrawModel(models_[i][j]);
			}
		}
	}
}






