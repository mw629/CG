#pragma once
#include "Core/VariableTypes.h"

struct ModelList{
	ModelData modelData;
	std::string directoryPath;
	std::string filename;
};

class ModelManager
{
private:
	
	ModelList modelList_;

	//頂点データ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	VertexData* vertexData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpDataResource_;
	TransformationMatrix* wvpData_ = nullptr;
	int vertexSize_ = 0;

public:
	ModelManager();

	void SetModelList(ModelData modelData,const std::string& directoryPath, const std::string& filename);

	bool DuplicateConfirmation(const std::string& directoryPath, const std::string& filename);

	ModelData DuplicateReturn(const std::string& directoryPath, const std::string& filename);

	Mesh CreateVertexData(std::vector<VertexData> vertices);
};

