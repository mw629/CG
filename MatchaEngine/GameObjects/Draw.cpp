#include "Draw.h"
#include "Common/GraphicsDevice.h"
#include <cassert>


Draw::Draw(ID3D12GraphicsCommandList* commandList)
{
	commandList_ = commandList;
}

Draw::~Draw()
{
	Initialize();
}

void Draw::Initialize()
{
	
}



void Draw::DrawObj(Model* model)
{
	//objectの描画
	commandList_->IASetVertexBuffers(0, 1, model->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, model->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, model->GetWvpDataResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, model->GetTextureSrvHandleGPU());

	commandList_->DrawInstanced(UINT(model->GetModelData().vertices.size()), 1, 0, 0);
}
