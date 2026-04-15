#pragma once
#include "Core/VariableTypes.h"

#include <unordered_map>

class ModelManager
{
private:

	Mesh mesh_;

	//頂点データ

public:
	ModelManager();

	void SetModelList(ModelData& modelData,const std::string& directoryPath, const std::string& filename);

	bool DuplicateConfirmation(const std::string& directoryPath, const std::string& filename);

	ModelData DuplicateReturn(const std::string& directoryPath, const std::string& filename);

	static ModelData GetModelData(int modelNumber);

	Mesh CreateMesh(std::vector<VertexData> vertices, std::vector<int32_t>indices);
	void CreateVertexData(std::vector<VertexData> vertices);
	void CreateIndexResource(std::vector<int32_t>indices);
};

