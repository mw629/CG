#pragma once
#include "Core/VariableTypes.h"

struct ModelList{
	ModelData modelData;
	std::string directoryPath;
	std::string filename;
};

class OBJManager
{
private:
	
	ModelList modelList_;

public:
	OBJManager();

	void SetModelList(ModelData modelData,const std::string& directoryPath, const std::string& filename);

	bool DuplicateConfirmation(const std::string& directoryPath, const std::string& filename);

	ModelData DuplicateReturn(const std::string& directoryPath, const std::string& filename);
};

