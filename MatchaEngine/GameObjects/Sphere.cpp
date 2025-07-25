#include "Sphere.h"
#include "Graphics/GraphicsDevice.h"
#include "Core/VariableTypes.h"
#include "Math/Calculation.h"

namespace {
	ID3D12Device* device_;
}


void Sphere::SetDevice(ID3D12Device* device)
{
	device_ = device;
}

Sphere::Sphere()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
}

Sphere::~Sphere()
{
	if (vertexData_) { // vertexData_がnullptrでないか確認
		vertexResource_->Unmap(0, nullptr);
	}
	if (indexData_) { // indexData_がnullptrでないか確認
		indexResource_->Unmap(0, nullptr);
	}
	if (wvpData_) { // wvpData_がnullptrでないか確認
		wvpResource_->Unmap(0, nullptr);
	}
	// ComPtrはスコープを抜けるときに自動的に解放されますが、
	// 明示的にReset()を呼ぶことで早期に解放できます（必須ではないが安全策）
	vertexResource_.Reset();
	indexResource_.Reset();
	wvpResource_.Reset();

	delete material_;
}




void Sphere::Initialize( D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU)
{
	textureSrvHandleGPU_ = textureSrvHandleGPU;

	material_ = new MaterialFactory();
	material_->CreateMatrial(device_, false);
}

void Sphere::CreateVertexData()
{

	float pi = 3.14f;


	//Shpere用の頂点リソースを作る//

	vertexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(VertexData) * (kSubdivision_ * kSubdivision_) * 6);

	//頂点バッファービューを作成する

	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * (kSubdivision_ * kSubdivision_) * 6;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点データを設定する//

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//経度分割1つ分の角度φd
	const float kLonEvery = pi * 2.0f / float(kSubdivision_);
	//緯度分割1つ分の角度Θd
	const float kLatEvery = pi / float(kSubdivision_);
	//緯度の方向に分割	
	for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//Θ
		//経度方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
			uint32_t startIndex = (latIndex * kSubdivision_ + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;

			//一つ目の三角形

			VertexData a;
			VertexData b;
			VertexData c;
			VertexData d;

			//position

			a.position = {
				cos(lat) * cos(lon),
				sin(lat),
				cos(lat) * sin(lon),
				1.0f };

			b.position = {
				cos(lat + kLatEvery) * cos(lon),
				sin(lat + kLatEvery),
				cos(lat + kLatEvery) * sin(lon) ,
				1.0f };

			c.position = {
				cos(lat) * cos(lon + kLonEvery),
				sin(lat),
				cos(lat) * sin(lon + kLonEvery),
				1.0f };

			d.position = {
				cos(lat + kLatEvery) * cos(lon + kLonEvery),
				sin(lat + kLatEvery),
				cos(lat + kLatEvery) * sin(lon + kLonEvery),
				1.0f };

			//texcoord
			a.texcoord = {
			float(lonIndex) / float(kSubdivision_),
			1.0f - float(latIndex) / float(kSubdivision_) };

			b.texcoord = {
				float(lonIndex) / float(kSubdivision_),
				1.0f - float(latIndex + 1) / float(kSubdivision_) };

			c.texcoord = {
				float(lonIndex + 1) / float(kSubdivision_),
				1.0f - float(latIndex) / float(kSubdivision_) };

			d.texcoord = {
				float(lonIndex + 1) / float(kSubdivision_),
				1.0f - float(latIndex + 1) / float(kSubdivision_) };

			//法線ベクトルを計算する

			a.normal = Vector3(a.position.x, a.position.y, a.position.z);
			b.normal = Vector3(b.position.x, b.position.y, b.position.z);
			c.normal = Vector3(c.position.x, c.position.y, c.position.z);
			d.normal = Vector3(d.position.x, d.position.y, d.position.z);


			//頂点にデータを入力する。基準点a
			vertexData_[startIndex] = b;
			vertexData_[startIndex + 1] = c;
			vertexData_[startIndex + 2] = a;
			//二つ目の三角形
			vertexData_[startIndex + 3] = d;
			vertexData_[startIndex + 4] = c;
			vertexData_[startIndex + 5] = b;


		}
	}


}

void Sphere::CreateIndexResource()
{
	indexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(uint32_t) * 6); ;


	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;

}

void Sphere::CreateWVP()
{

	//Sprite用ののTransformationMatrix用のリソースを作る。Matrix4x41つ分のサイズを用意する
	wvpResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(TransformationMatrix));
	//データを書き込む
	wvpData_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列をかきこんでおく
	wvpData_->World = IdentityMatrix();
	wvpData_->WVP = IdentityMatrix();
}

void Sphere::CreateSprite()
{
	CreateVertexData();
	//CreateIndexResource(device);
	CreateWVP();
}

void Sphere::SettingWvp(Matrix4x4 viewMatrix)
{
	Matrix4x4 projectionMatri = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatri));
	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };
}

void Sphere::SetTrandform(Transform transform)
{
	transform_.translate = transform.translate;
	transform_.scale = transform.scale;
	transform_.rotate = transform.rotate;
}
