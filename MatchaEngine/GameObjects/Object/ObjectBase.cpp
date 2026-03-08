#include "ObjectBase.h"
#include "Calculation.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#endif // _USE_IMGUI


// 静的メンバーの定義
float ObjectBase::kClientWidth_ = 0.0f;
float ObjectBase::kClientHeight_ = 0.0f;

ObjectBase::~ObjectBase()
{

}

void ObjectBase::ImGui() {
#ifdef _USE_IMGUI
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::DragFloat3("Position", reinterpret_cast<float*>(&transform_.translate), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&transform_.rotate), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
		ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&transform_.scale), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
	}
#endif // _USE_IMGUI
}


void ObjectBase::SetObjectResource(Vector2 ClientSize)
{
	kClientWidth_ = ClientSize.x;
	kClientHeight_ = ClientSize.y;
}

void ObjectBase::CreateWVP()
{
	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	wvpDataResource_ = GraphicsDevice::CreateBufferResource(sizeof(TransformationMatrix));
	//データを書き込む

	//書き込むためのアドレスを取得
	wvpDataResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->WVP = IdentityMatrix();
	wvpData_->World = IdentityMatrix();
	wvpData_->WorldInverseTranspose = IdentityMatrix();;
}


void ObjectBase::SettingWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatrix));
	Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldMatrix)); // 法線変換用の行列を計算

	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = worldMatrix;
	wvpData_->WorldInverseTranspose = worldInverseTranspose;

};

void ObjectBase::CreateObject()
{
	CreateWVP();
}


