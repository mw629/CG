#include "Model.h"
#include "GraphicsDevice.h"
#include "Calculation.h"
#include "Load.h"
#include "Texture.h"
#include "ModelManager.h"
#include <fstream>
#include <sstream>


Model::~Model()
{
	// 基底クラス(Object3DBase)のデストラクタが自動的に呼ばれる
	// materialはunique_ptrなので自動的に解放される
}


void Model::Initialize(ModelData modelData)
{

	modelNumber_ = modelData.modelNumber;
	textureSrvHandleGPU_ = texture->TextureData(modelData.textureIndex);

	//アニメーション
	rootNode_ = modelData.rootNode;

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMartial();
	CreateObject();
}


void Model::SettingWvp(Matrix4x4 viewMatrix) {

	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 10000.0f);
	Matrix4x4 worldMatrixObj = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldMatrixObj));


	wvpData_->WVP = MultiplyMatrix4x4(rootNode_.localMatrix, worldViewProjectionMatrixObj);
	wvpData_->World = MultiplyMatrix4x4(rootNode_.localMatrix, worldMatrixObj);
	wvpData_->WorldInverseTranspose = worldInverseTranspose;
}



void Model::CreateObject()
{
	CreateWVP();
}

Mesh Model::GetMesh()
{
	ModelData data = ModelManager::GetModelData(modelNumber_);
	return data.mesh;
}

