#include "Object3DBase.h"
#include "Calculation.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI


// 静的メンバーの定義
ID3D12Device* Object3DBase::device_ = nullptr;
float Object3DBase::kClientWidth_ = 0.0f;
float Object3DBase::kClientHeight_ = 0.0f;

Object3DBase::~Object3DBase()
{

}

void Object3DBase::ImGui() {
#ifdef _USE_IMGUI
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::DragFloat3("Position", reinterpret_cast<float*>(&transform_.translate), 0.01f);
		ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&transform_.rotate), 0.01f);
		ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&transform_.scale), 0.01f);
	}
#endif // _USE_IMGUI
}


void Object3DBase::SetObjectResource(ID3D12Device* device, Vector2 ClientSize)
{
	device_ = device;
	kClientWidth_ = ClientSize.x;
	kClientHeight_ = ClientSize.y;
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
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };

};

void Object3DBase::CreateObject()
{
	CreateVertexData();
	CreateWVP();
}
