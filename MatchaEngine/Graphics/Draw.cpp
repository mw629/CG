#include "Draw.h"
#include "Graphics/GraphicsDevice.h"
#include <cassert>

namespace {
	ID3D12GraphicsCommandList* commandList_{};
}

void Draw::Initialize(ID3D12GraphicsCommandList* commandList)
{
	commandList_ = commandList;
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

void Draw::DrawShpere(Sphere* sphere)
{
	//commandList_->IASetIndexBuffer(shpere->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sphere->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, sphere->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, sphere->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, sphere->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(static_cast<UINT>(pow(sphere->GetSubdivision(), 2) * 6), 1, 0, 0);
}

void Draw::DrawTriangle(Triangle* triangle)
{
	commandList_->IASetVertexBuffers(0, 1, triangle->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, triangle->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, triangle->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, triangle->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(3, 1, 0, 0);
}



void Draw::DrawLine(Line* line)
{
	commandList_->IASetVertexBuffers(0, 1, line->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, line->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->DrawInstanced(2, 1, 0, 0);
}

void Draw::DrawGrid(Grid* grid)
{
	commandList_->IASetVertexBuffers(0, 1, grid->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, grid->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->DrawInstanced(grid->GetSubdivision() * 4, 1, 0, 0);
}

