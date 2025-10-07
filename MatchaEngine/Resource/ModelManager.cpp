#include "ModelManager.h"

namespace {
	std::vector<ModelList> modelList;
}

ModelManager::ModelManager()
{
}

void ModelManager::SetModelList(ModelData modelData, const std::string& directoryPath, const std::string& filename)
{
	modelList_.modelData = modelData;
	modelList_.directoryPath = directoryPath;
	modelList_.filename = filename;
	modelList.push_back(modelList_);
}

bool ModelManager::DuplicateConfirmation(const std::string& directoryPath, const std::string& filename)
{
	for (int i = 0; i < modelList.size(); i++) {
		if (modelList[i].directoryPath == directoryPath) {
			if (modelList[i].filename == filename) {
				return true;
			}
		}
	}
	return false;
}

ModelData ModelManager::DuplicateReturn(const std::string& directoryPath, const std::string& filename)
{
	for (int i = 0; i < modelList.size(); i++) {
		if (modelList[i].directoryPath == directoryPath) {
			if (modelList[i].filename == filename) {
				return modelList[i].modelData;
			}
		}
	}
	return modelList[0].modelData;
}
