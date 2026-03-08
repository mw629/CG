#include "ModelManager.h"
#include "GraphicsDevice.h"
#include <Calculation.h>

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

ModelData ModelManager::GetModelData(int modelNumber)
{
	for (int i = 0; i < modelList.size(); i++) {
		if (modelList[i].modelData.modelNumber == modelNumber) {
			return modelList[i].modelData;
		}
	}
	return modelList[0].modelData;
}

Mesh ModelManager::CreateMesh(std::vector<VertexData> vertices, std::vector<int32_t>indices)
{
	CreateVertexData(vertices);
	CreateIndexResource(indices);
	return mesh_;
}

void ModelManager::CreateVertexData(std::vector<VertexData> vertices)
{
	// 頂点リソースを作成

	mesh_.vertexResource = GraphicsDevice::CreateBufferResource(sizeof(VertexData) * vertices.size());

	// BufferViewを設定
	mesh_.vertexBufferView.BufferLocation = mesh_.vertexResource->GetGPUVirtualAddress();
	mesh_.vertexBufferView.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * vertices.size());
	mesh_.vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点データをマップしてコピー
	VertexData* vertexData = nullptr;
	mesh_.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());

	mesh_.vertexSize = static_cast<int>(vertices.size());

}

void ModelManager::CreateIndexResource(std::vector<int32_t>indices)
{
	mesh_.indexResource = GraphicsDevice::CreateBufferResource(sizeof(uint32_t) * indices.size());
	mesh_.indexBufferView.BufferLocation = mesh_.indexResource->GetGPUVirtualAddress();
	mesh_.indexBufferView.SizeInBytes = sizeof(uint32_t) * indices.size();
	mesh_.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = nullptr;
	mesh_.indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());

	mesh_.indexSize = mesh_.indexBufferView.SizeInBytes / sizeof(uint32_t);

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;

}

