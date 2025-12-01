#include "Model.h"
#include "Graphics/GraphicsDevice.h"
#include "Math/Calculation.h"
#include "Resource/Load.h"
#include <fstream>
#include <sstream>
#include "Particle.h"


namespace {
	ID3D12Device* device_;
	float kClientWidth;
	float kClientHeight;
}


void Model::SetDevice(ID3D12Device* device)
{
	device_ = device;
}

void Model::SetScreenSize(Vector2 screenSize)
{
	kClientWidth = screenSize.x;
	kClientHeight = screenSize.y;
}

Model::~Model()
{
	// MapしたポインタをUnmapする
	if (vertexData_) {
		vertexResource_->Unmap(0, nullptr);
	}
	if (wvpData_) {
		wvpDataResource_->Unmap(0, nullptr);
	}
	// ComPtrは自動的に解放される
	vertexResource_.Reset();
	wvpDataResource_.Reset();

}


void Model::Initialize(ModelData modelData)
{
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	transform_ = {};
	modelData_ = modelData;
	textureSrvHandleGPU_ = texture->TextureData(modelData.textureIndex);

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMatrial(device_, false);
	CreateModel();
}


void Model::CreateVertexData()
{
	//頂点リソースを作る
	vertexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(VertexData) * modelData_.vertices.size());
	//頂点バッファービューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}


void Model::CreateWVP()
{
	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	wvpDataResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(TransformationMatrix));
	//データを書き込む

	//書き込むためのアドレスを取得
	wvpDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->WVP = IdentityMatrix();
	wvpData_->World = IdentityMatrix();
}

void Model::CreateModel()
{
	CreateVertexData();
	CreateWVP();
}


void Model::SettingWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldMatrixObj = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrixObj = MultiplyMatrix4x4(worldMatrixObj, MultiplyMatrix4x4(viewMatrix, projectionMatri));

	wvpData_->WVP = worldViewProjectionMatrixObj * modelData_.rootNode.localMatrix;
	wvpData_->World = worldMatrixObj * modelData_.rootNode.localMatrix;
};


void Model::SetTransform(Transform transform)
{
	transform_.translate = transform.translate;
	transform_.scale = transform.scale;
	transform_.rotate = transform.rotate;
}
