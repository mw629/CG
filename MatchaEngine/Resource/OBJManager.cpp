#include "OBJManager.h"

namespace {
	std::vector<ModelList> modelList;
}

OBJManager::OBJManager()
{
}

void OBJManager::SetModelList(ModelData modelData, const std::string& directoryPath, const std::string& filename)
{
	modelList_.modelData = modelData;
	modelList_.directoryPath = directoryPath;
	modelList_.filename = filename;
	modelList.push_back(modelList_);
}

bool OBJManager::DuplicateConfirmation(const std::string& directoryPath, const std::string& filename)
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

ModelData OBJManager::DuplicateReturn(const std::string& directoryPath, const std::string& filename)
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
