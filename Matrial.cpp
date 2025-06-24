#include "Matrial.h"
#include "MatchaEngine/Common/GraphicsDevice.h"
#include "MatchaEngine/Math/Calculation.h"

void Matrial::CreateMatrial(ID3D12Device* device)
{
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource = GraphicsDevice::CreateBufferResource(device, sizeof(Material));
	//マテリアルデータを書き込む
	materialData = nullptr;
	//書き込むためのアドレス取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は書き込んでみる
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->endbleLighting = true;
	materialData->uvTransform = IdentityMatrix();
}
