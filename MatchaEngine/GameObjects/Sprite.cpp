#include "Sprite.h"
#include "Graphics/GraphicsDevice.h"
#include "Core/VariableTypes.h"
#include "Math/Calculation.h"
#include <Resource/Texture.h>

namespace {
	ID3D12Device* device_;
	float kClientWidth;
	float kClientHeight;
}

void Sprite::SetDevice(ID3D12Device* device) {
	device_ = device;
}

void Sprite::SetScreenSize(Vector2 screenSize)
{
	kClientWidth = screenSize.x;
	kClientHeight = screenSize.y;
}


Sprite::Sprite()
{
	transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
}

Sprite::~Sprite()
{
	// MapしたポインタをUnmapする
	if (vertexData_) {
		vertexResource_->Unmap(0, nullptr);
	}
	if (indexData_) {
		indexResource_->Unmap(0, nullptr);
	}
	if (wvpData_) {
		wvpResource_->Unmap(0, nullptr);
	}
	// ComPtrは自動的に解放される
	vertexResource_.Reset();
	indexResource_.Reset();
	wvpResource_.Reset();

	delete material_;
}

void Sprite::Initialize(SpriteData spriteData, int textureHandle)
{
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	textureSrvHandleGPU_ = texture.get()->TextureData(textureHandle);

	SetTransform(spriteData.transform);
	SetSize(spriteData.size);
	SetTextureArea(spriteData.textureArea);


	material_ = new MaterialFactory();
	material_->CreateMatrial(device_, false);
	CreateSprite();
}

void Sprite::CreateVertexData()
{
	//Sprote用の頂点リソースを作る//

	vertexResource_ = GraphicsDevice::CreateBufferResource(device_, sizeof(VertexData) * 6);

	//頂点バッファービューを作成する
	
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);


	//頂点データを設定する//
	
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//１枚目の三角形

	leftTop_.x = transform_.translate.x - (size_.x / 2.0f);
	leftTop_.y = transform_.translate.y - (size_.y / 2.0f);
	rigthBottom_.x = transform_.translate.x + (size_.x / 2.0f);
	rigthBottom_.y = transform_.translate.y + (size_.y / 2.0f);

	//１枚目の三角形
	vertexData_[0].position = { leftTop_.x,rigthBottom_.y,0.0f,1.0f };//左下
	vertexData_[0].texcoord = { textureArea_[0].x,textureArea_[1].y };
	vertexData_[1].position = { leftTop_.x,leftTop_.y,0.0f,1.0f };//左上
	vertexData_[1].texcoord = { textureArea_[0].x,textureArea_[0].y };
	vertexData_[2].position = { rigthBottom_.x,rigthBottom_.y,0.0f,1.0f };//右下
	vertexData_[2].texcoord = { textureArea_[1].x,textureArea_[1].y };
	//2枚目の三角形
	vertexData_[3].position = { rigthBottom_.x,leftTop_.y,0.0f,1.0f };//右上
	vertexData_[3].texcoord = { textureArea_[1].x,textureArea_[0].y };


}

void Sprite::CreateIndexResource()
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

void Sprite::CreateWVP()
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

void Sprite::CreateSprite()
{
	CreateVertexData();
	CreateIndexResource();
	CreateWVP();
}

void Sprite::SettingWvp()
{
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(IdentityMatrix(), MakeOrthographicMatrix(0, float(kClientWidth), 0, float(kClientHeight), 0.0f, 100.0f)));
	*wvpData_ = { worldViewProjectionMatrix,worldMatrix };

}

void Sprite::SetTransform(Transform transform) {
	transform_ = transform;
}

void Sprite::SetSize(Vector2 size)
{
	size_ = size;
}

SpriteData Sprite::SetData(Vector2* const size)
{
	SpriteData data;
	data.transform = transform_;
	data.size = *size;
	data.textureArea[0] = textureArea_[0];
	data.textureArea[1] = textureArea_[1];
	return data;
}

void Sprite::SetTextureArea(Vector2 textureArea[2])
{
	textureArea_[0] = textureArea[0];
	textureArea_[1] = textureArea[1];
}

void Sprite::Update(SpriteData spriteData)
{
	SetTransform(spriteData.transform);
	SetSize(spriteData.size);
	SetTextureArea(spriteData.textureArea);

	leftTop_.x = transform_.translate.x - (size_.x / 2.0f);
	leftTop_.y = transform_.translate.y - (size_.y / 2.0f);
	rigthBottom_.x = transform_.translate.x + (size_.x / 2.0f);
	rigthBottom_.y = transform_.translate.y + (size_.y / 2.0f);

	//１枚目の三角形
	vertexData_[0].position = { leftTop_.x,rigthBottom_.y,0.0f,1.0f };//左下
	vertexData_[0].texcoord = { textureArea_[0].x,textureArea_[1].y};
	vertexData_[1].position = { leftTop_.x,leftTop_.y,0.0f,1.0f };//左上
	vertexData_[1].texcoord = { textureArea_[0].x,textureArea_[0].y };
	vertexData_[2].position = { rigthBottom_.x,rigthBottom_.y,0.0f,1.0f };//右下
	vertexData_[2].texcoord = { textureArea_[1].x,textureArea_[1].y };
	//2枚目の三角形
	vertexData_[3].position = { rigthBottom_.x,leftTop_.y,0.0f,1.0f };//右上
	vertexData_[3].texcoord = { textureArea_[1].x,textureArea_[0].y };

	SettingWvp();
}
