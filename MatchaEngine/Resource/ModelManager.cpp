#include "ModelManager.h"
#include "GraphicsDevice.h"

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

Mesh ModelManager::CreateVertexData(std::vector<VertexData> vertices)
{
	Mesh mesh;
	
	// 頂点リソースを作成
	mesh.vertexResource = GraphicsDevice::CreateBufferResource(sizeof(VertexData) * vertices.size());
	
	// BufferViewを設定
	mesh.vertexBufferView.BufferLocation = mesh.vertexResource->GetGPUVirtualAddress();
	mesh.vertexBufferView.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * vertices.size());
	mesh.vertexBufferView.StrideInBytes = sizeof(VertexData);
	
	// 頂点データをマップしてコピー
	VertexData* vertexData = nullptr;
	mesh.vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
	
	mesh.vertexSize = static_cast<int>(vertices.size());
	
	return mesh;
}
