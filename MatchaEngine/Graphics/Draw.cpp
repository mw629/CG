#include "Draw.h"
#include "Graphics/GraphicsDevice.h"
#include <cassert>

namespace {
	ID3D12GraphicsCommandList* commandList_{};
	GraphicsPipelineState* graphicsPipelineState_;
	DirectinalLight* directinalLight_;
}

void Draw::Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState, DirectinalLight* directinalLight)
{
	commandList_ = commandList;
	graphicsPipelineState_ = graphicsPipelineState;
	directinalLight_ = directinalLight;
}

void Draw::preDraw(ShaderName shader, BlendMode blend)
{
	commandList_->SetPipelineState(graphicsPipelineState_->GetGraphicsPipelineState(shader, blend));//PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//RootSignatureを設定。POSに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(graphicsPipelineState_->GetRootSignature(shader, blend)->GetRootSignature());


}

void Draw::DrawModel(Model* model, Camera* camera)
{
	preDraw(model->GetShader(), model->GetBlend());
	commandList_->SetGraphicsRootConstantBufferView(3, directinalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, camera->GetCameraResource()->GetGPUVirtualAddress());

	//objectの描画
	commandList_->IASetVertexBuffers(0, 1, model->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, model->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, model->GetWvpDataResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, model->GetTextureSrvHandleGPU());

	commandList_->DrawInstanced(UINT(model->GetModelData().vertices.size()), 1, 0, 0);
}

void Draw::DrawParticle(Particle* particle)
{
	// インスタンス数が0なら描画しない
	const UINT instanceCount = static_cast<UINT>(particle->GetParticleNum());
	if (instanceCount == 0) {
		return;
	}

	preDraw(particle->GetShader(), particle->GetBlend());

	commandList_->IASetVertexBuffers(0, 1, particle->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, particle->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(1, particle->GetInstancingSrvHandleGPU());
	commandList_->SetGraphicsRootDescriptorTable(2, particle->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(6, instanceCount, 0, 0);

}

void Draw::DrawSprite(Sprite* sprite)
{
	preDraw(sprite->GetShader(), sprite->GetBlend());
	commandList_->SetGraphicsRootConstantBufferView(3, directinalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());

	commandList_->IASetIndexBuffer(sprite->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sprite->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, sprite->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, sprite->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, sprite->GetTextureSrvHandleGPU());

	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Draw::DrawShpere(Sphere* sphere, Camera* camera)
{

	preDraw(sphere->GetShader(), sphere->GetBlend());
	commandList_->SetGraphicsRootConstantBufferView(3, directinalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, camera->GetCameraResource()->GetGPUVirtualAddress());

	//commandList_->IASetIndexBuffer(shpere->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sphere->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, sphere->GetMatrial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, sphere->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, sphere->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(static_cast<UINT>(pow(sphere->GetSubdivision(), 2) * 6), 1, 0, 0);
}

void Draw::DrawTriangle(Triangle* triangle, Camera* camera)
{
	preDraw(triangle->GetShader(), triangle->GetBlend());
	commandList_->SetGraphicsRootConstantBufferView(3, directinalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, camera->GetCameraResource()->GetGPUVirtualAddress());

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

