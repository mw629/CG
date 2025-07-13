#include "Line.h"
#include "Graphics/GraphicsDevice.h"
#include "Math/Calculation.h"

void Line::Initialize(MaterialFactory* material, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) {
	material_ = material;
	textureSrvHandleGPU_ = textureSrvHandleGPU;
	vertex_[0] = { -0.1f,-0.1f,0.0f,1.0f };
	vertex_[1] = { 0.0f,0.1f,0.0f,1.0f };
	vertex_[2] = { 0.1f,-0.1f,0.0f,1.0f };
}



void Line::CreateVertexData(ID3D12Device* device)
{
	//VertexResourceを生成する//
	vertexResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(VertexData) * 2);
	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 2;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//Resouceにデータを書き込む//

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//左下
	vertexData_[0].position = vertex_[0];
	vertexData_[0].texcoord = { 0.0f,0.0f };
	//上
	vertexData_[1].position = vertex_[1];
	vertexData_[1].texcoord = { 1.0f,1.0f };
}

void Line::CreateWVP(ID3D12Device* device) {
	//WVP用のリソースを作る
	wvpResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(TransformationMatrix));
	//
	wvpData_ = nullptr;
	//
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//
	wvpData_->WVP = IdentityMatrix();
	wvpData_->World = IdentityMatrix();
}

void Line::CreateTriangle(ID3D12Device* device)
{
	CreateVertexData(device);
	CreateWVP(device);
}

void Line::SetWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };
}

void Line::SetShape()
{
	//左下
	vertexData_[0].position = vertex_[0];
	vertexData_[0].texcoord = { 0.0f,0.0f };
	//上
	vertexData_[1].position = vertex_[1];
	vertexData_[1].texcoord = { 1.0f,1.0f };
	
}


void Line::SetTrandform(Transform transform)
{
	transform_.translate = transform.translate;
	transform_.scale = transform.scale;
	transform_.rotate = transform.rotate;
}

void Line::SetVertex(Vector4 vertex[3])
{
	for (int i = 0; i < 2; i++) {
		vertex_[i] = vertex[i];
	}
}
