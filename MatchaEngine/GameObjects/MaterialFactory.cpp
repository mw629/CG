#include "MaterialFactory.h"
#include "Graphics/GraphicsDevice.h"
#include "Math/Calculation.h"


MaterialFactory::~MaterialFactory(){
	if (materialResource_) {
		materialResource_->Unmap(0, nullptr);
		materialResource_.Reset();
	}
}

void MaterialFactory::CreateMatrial(ID3D12Device* device,bool Lighting)
{
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = GraphicsDevice::CreateBufferResource(device, sizeof(Material));
	//書き込むためのアドレス取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//今回は書き込んでみる
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->endbleLighting = Lighting;
	materialData_->uvTransform = IdentityMatrix();
	materialData_->shininess = 30.0f;
}
