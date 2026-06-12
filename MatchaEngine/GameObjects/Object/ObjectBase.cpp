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
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader(name_.c_str())) {
		if (ImGui::TreeNode("Transform")) {
			ImGui::DragFloat3("Position", reinterpret_cast<float*>(&transform_.translate), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
			ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&transform_.rotate), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
			ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&transform_.scale), 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("Shader")) {
			const char* shaders[] = { "ObjectShader", "IceShader", "AnimationObj", "SkyBoxShader" };
			int current_item = 0;
			for (int i = 0; i < IM_ARRAYSIZE(shaders); ++i) {
				if (shader_ == shaders[i]) {
					current_item = i;
					break;
				}
			}
			if (ImGui::Combo("Shader Selection", &current_item, shaders, IM_ARRAYSIZE(shaders))) {
				shader_ = shaders[current_item];
			}
			ImGui::TreePop();
		}

		// Blend mode UI
	if (ImGui::TreeNode("Blend")) {
		static const BlendMode blendModes[] = { kBlendModeNone, kBlendModeNormal, kBlendModeAdd, kBlendModeSubtract, kBlendModeMultiply, kBlendModeScreen };
		static const char* blendNames[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
		int current_blend = 0;
		for (int i = 0; i < IM_ARRAYSIZE(blendModes); ++i) {
			if (blend_ == blendModes[i]) { current_blend = i; break; }
		}
		if (ImGui::Combo("Blend Selection", &current_blend, blendNames, IM_ARRAYSIZE(blendNames))) {
			blend_ = blendModes[current_blend];
		}
		ImGui::TreePop();
	}
	material_.get()->ImGui();
	}
	ImGui::PopID();
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

void ObjectBase::CreateIndexResource()
{
	indexResource_ = GraphicsDevice::CreateBufferResource(sizeof(uint32_t) * 6); ;


	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexSize_ = sizeof(uint32_t) * 6;

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

}


void ObjectBase::SettingWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldMatrix)); // 法線変換用の行列を計算

	wvpData_->WVP = worldViewProjectionMatrix;
	wvpData_->World = worldMatrix;
	wvpData_->WorldInverseTranspose = worldInverseTranspose;

};

void ObjectBase::CreateObject()
{
	CreateVertexData();
	CreateWVP();
	CreateIndexResource();
}

void ObjectBase::CreateVertexData()
{
	// 基底クラスのデフォルト実装（派生クラスでオーバーライドされる）
}

Mesh ObjectBase::GetMesh()
{
	Mesh mesh;
	mesh.vertexBufferView = vertexBufferView_;
	mesh.vertexSize = vertexSize_;
	mesh.indexBufferView_ = indexBufferView_;
	return mesh;
}

D3D12_VERTEX_BUFFER_VIEW* ObjectBase::GetVertexBufferView()
{
	return &vertexBufferView_;
}
