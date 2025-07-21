#include "Triangle.h"
#include "Graphics/GraphicsDevice.h"
#include "Math/Calculation.h"
#include "Line.h"


Triangle::~Triangle()
{
	// MapしたポインタをUnmapする
	if (vertexData_) {
		vertexResource_->Unmap(0, nullptr);
	}
	if (wvpData_) {
		wvpResource_->Unmap(0, nullptr);
	}
	// ComPtrは自動的に解放される
	vertexResource_.Reset();
	wvpResource_.Reset();
}

namespace {
	ID3D12Device* device_;
}

void Triangle::SetDevice(ID3D12Device* device) {
	device_ = device;
}

void Triangle::Initialize(MaterialFactory* material, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) {
	material_ = material;
	textureSrvHandleGPU_ = textureSrvHandleGPU;
	vertex_[0] = { -0.1f,-0.1f,0.0f,1.0f };
	vertex_[1] = { 0.0f,0.1f,0.0f,1.0f };
	vertex_[2] = { 0.1f,-0.1f,0.0f,1.0f };
}



void Triangle::CreateVertexData()
{
	//VertexResourceを生成する//
	vertexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(VertexData) * 3);
	//頂点バッファビューを作成する
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//Resouceにデータを書き込む//

	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//左下
	vertexData_[0].position = vertex_[0];
	vertexData_[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData_[1].position = vertex_[1];
	vertexData_[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData_[2].position = vertex_[2];
	vertexData_[2].texcoord = { 1.0f,1.0f };
}

void Triangle::CreateWVP() {
	//WVP用のリソースを作る
	wvpResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(TransformationMatrix));
	//
	wvpData_ = nullptr;
	//
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//
	wvpData_->WVP = IdentityMatrix();
	wvpData_->World = IdentityMatrix();
}



void Triangle::CreateTriangle()
{
	CreateVertexData();
	CreateWVP();
}

void Triangle::SetWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };
}

void Triangle::SetShape()
{
	//左下
	vertexData_[0].position = vertex_[0];
	vertexData_[0].texcoord = { 0.0f,1.0f };
	//上
	vertexData_[1].position = vertex_[1];
	vertexData_[1].texcoord = { 0.5f,0.0f };
	//右下
	vertexData_[2].position = vertex_[2];
	vertexData_[2].texcoord = { 1.0f,1.0f };
}


void Triangle::SetTrandform(Transform transform)
{
	transform_.translate = transform.translate;
	transform_.scale = transform.scale;
	transform_.rotate = transform.rotate;
}

void Triangle::SetVertex(Vector4 vertex[3])
{
	for (int i = 0; i < 3; i++) {
		vertex_[i] = vertex[i];
	}
}
