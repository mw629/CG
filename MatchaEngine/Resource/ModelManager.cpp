#include "ModelManager.h"
#include "GraphicsDevice.h"
#include <unordered_map>

namespace {
	std::unordered_map<std::string, ModelData> modelMap;
	int modelNumber=0;
}

ModelManager::ModelManager()
{
}

void ModelManager::SetModelList(ModelData modelData, const std::string& directoryPath, const std::string& filename)
{
	modelData.modelNumber = modelNumber;
	std::string key = directoryPath + "/" + filename;
	modelMap[key] = modelData;
	modelNumber++;
}

bool ModelManager::DuplicateConfirmation(const std::string& directoryPath, const std::string& filename)
{
	std::string key = directoryPath + "/" + filename;
	return modelMap.find(key) != modelMap.end();
}

ModelData ModelManager::DuplicateReturn(const std::string& directoryPath, const std::string& filename)
{
	std::string key = directoryPath + "/" + filename;
	auto it = modelMap.find(key);
	if (it != modelMap.end()) {
		return it->second;
	}
	return modelMap.begin()->second;
}

ModelData ModelManager::GetModelData(int modelNumber)
{
	for (auto& pair : modelMap) {
		if (pair.second.modelNumber == modelNumber) {
			return pair.second;
		}
	}
	return modelMap.begin()->second;
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
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	vertexResource = GraphicsDevice::CreateBufferResource(sizeof(VertexData) * vertices.size());
	
	// BufferViewを設定
	mesh_.vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	mesh_.vertexBufferView.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * vertices.size());
	mesh_.vertexBufferView.StrideInBytes = sizeof(VertexData);
	
	// 頂点データをマップしてコピー
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
	
	mesh_.vertexSize = static_cast<int>(vertices.size());
	
}

void ModelManager::CreateIndexResource(std::vector<int32_t>indices)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	indexResource = GraphicsDevice::CreateBufferResource(sizeof(uint32_t) * indices.size());
	mesh_.indexBufferView_.BufferLocation = indexResource->GetGPUVirtualAddress();
	mesh_.indexBufferView_.SizeInBytes = sizeof(uint32_t) * indices.size();
	mesh_.indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData_ = nullptr; 
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, indices.data(), sizeof(uint32_t) * indices.size());

}
