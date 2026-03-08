#include "Model.h"
#include "GraphicsDevice.h"
#include "Calculation.h"
#include "Load.h"
#include "Texture.h"
#include <fstream>
#include <sstream>

#include "ModelManager.h"

Model::~Model()
{
	// 基底クラス(Object3DBase)のデストラクタが自動的に呼ばれる
	// materialはunique_ptrなので自動的に解放される
}


void Model::Initialize(int modelData)
{

	modelNumber_ = modelData;

	textureSrvHandleGPU_ = texture->TextureData(ModelManager::GetModelData(modelNumber_).textureIndex);

	//アニメーション
	
	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMartial();
	CreateObject();
}


void Model::SettingWvp(Matrix4x4 viewMatrix) {

	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 10000.0f);
	Matrix4x4 worldMatrixObj = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldMatrixObj));


	wvpData_->WVP = MultiplyMatrix4x4(ModelManager::GetModelData(modelNumber_).rootNode.localMatrix, worldViewProjectionMatrixObj);
	wvpData_->World = MultiplyMatrix4x4(ModelManager::GetModelData(modelNumber_).rootNode.localMatrix, worldMatrixObj);
	wvpData_->WorldInverseTranspose = worldInverseTranspose;
}



void Model::CreateObject()
{
	CreateWVP();
}

