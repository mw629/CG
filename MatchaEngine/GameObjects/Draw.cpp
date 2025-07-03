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

void Draw::DrawSprite(Sprite* sprite)
{
	commandList_->IASetIndexBuffer(sprite->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sprite->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, sprite->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, sprite->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, sprite->GetTextureSrvHandleGPU());
	
	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Draw::DrawShpere(Shpere* shpere)
{
	//commandList_->IASetIndexBuffer(shpere->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, shpere->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, shpere->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, shpere->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, shpere->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(pow(shpere->GetSubdivision(), 2) * 6, 1, 0, 0);
}
	
