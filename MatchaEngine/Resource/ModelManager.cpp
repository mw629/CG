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
	//頂点リソースを作る
	vertexResource_ = GraphicsDevice::CreateBufferResource(sizeof(VertexData) * vertices.size());
	//頂点バッファービューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, vertices.data(), sizeof(VertexData) * vertices.size());

	Mesh mesh;
	mesh.vertexBufferView = &vertexBufferView_;
	mesh.vertexSize = vertices.size();

	return mesh;
}
