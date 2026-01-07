#include "Object3DBase.h"
#include "Calculation.h"

namespace {
	ID3D12Device* device_;
	float kClientWidth;
	float kClientHeight;
}

Object3DBase::~Object3DBase()
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

void Object3DBase::SetObjectResource(ID3D12Device* device, Vector2 ClientSize)
{
	device_ = device;
	kClientWidth = ClientSize.x;
	kClientHeight = ClientSize.y;
}

void Object3DBase::CreateWVP()
{
	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	wvpDataResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(TransformationMatrix));
	//データを書き込む

	//書き込むためのアドレスを取得
	wvpDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->WVP = IdentityMatrix();
	wvpData_->World = IdentityMatrix();
	wvpData_->WorldInverseTranspose = IdentityMatrix();;
}

void Object3DBase::SettingWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };

};

void Object3DBase::CreateObject()
{
	CreateVertexData();
	CreateWVP();
}
