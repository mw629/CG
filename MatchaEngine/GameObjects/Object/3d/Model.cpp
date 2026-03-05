#include "Model.h"
#include "GraphicsDevice.h"
#include "Calculation.h"
#include "Load.h"
#include "Texture.h"
#include <fstream>
#include <sstream>


Model::~Model()
{
	// 基底クラス(Object3DBase)のデストラクタが自動的に呼ばれる
	// materialはunique_ptrなので自動的に解放される
}


void Model::Initialize(ModelData modelData)
{
	
	modelData_ = modelData;
	textureSrvHandleGPU_ = texture->TextureData(modelData.textureIndex);

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMartial();
	CreateObject();
}



void Model::SettingWvp(Matrix4x4 viewMatrix) {

	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 10000.0f);
	Matrix4x4 worldMatrixObj = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldMatrixObj));


	wvpData_->WVP = MultiplyMatrix4x4(modelData_.rootNode.localMatrix, worldViewProjectionMatrixObj);
	wvpData_->World = MultiplyMatrix4x4(modelData_.rootNode.localMatrix, worldMatrixObj);
	wvpData_->WorldInverseTranspose = worldInverseTranspose;
}

void Model::CreateIndexResource()
{
	indexResource_ = GraphicsDevice::CreateBufferResource(sizeof(uint32_t) * modelData_.indices.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());

}

void Model::CreateObject()
{
	CreateWVP();
	CreateIndexResource();
}

