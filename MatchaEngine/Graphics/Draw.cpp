#include "Draw.h"
#include "Graphics/GraphicsDevice.h"
#include <cassert>
#include "ModelManager.h"

namespace {
	ID3D12GraphicsCommandList* commandList_{};
	GraphicsPipelineState* graphicsPipelineState_;
	DirectionalLight* directionalLight_;
	PointLight* pointLight_;
	SpotLight* spotLight_;
	Camera* camera;
}

void Draw::Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState,
	DirectionalLight* directionalLight, PointLight* pointLight, SpotLight* spotLight)
{
	commandList_ = commandList;
	graphicsPipelineState_ = graphicsPipelineState;
	directionalLight_ = directionalLight;
	pointLight_ = pointLight;
	spotLight_ = spotLight;
}

void Draw::SetCamera(Camera* setCamera)
{
	camera = setCamera;
}

void Draw::preDraw(ShaderName shader, BlendMode blend)
{
	commandList_->SetPipelineState(graphicsPipelineState_->GetGraphicsPipelineState(shader, blend));//PSOを設定
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけばいい
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//RootSignatureを設定。POSに設定しているけど別途設定が必要
	commandList_->SetGraphicsRootSignature(graphicsPipelineState_->GetRootSignature(shader, blend)->GetRootSignature());


}

void Draw::DrawObj(ObjectBase* obj)
{
	preDraw(obj->GetShader(), obj->GetBlend());

	const ModelData& modelData = ModelManager::GetModelData(obj->GetModelNumber());

	//objectの描画
	commandList_->IASetIndexBuffer(&modelData.mesh.indexBufferView);
	commandList_->IASetVertexBuffers(0, 1, &modelData.mesh.vertexBufferView);
	commandList_->SetGraphicsRootConstantBufferView(0, obj->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, obj->GetWvpDataResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, obj->GetTextureSrvHandleGPU());
	commandList_->SetGraphicsRootConstantBufferView(3, camera->GetCameraResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());

	commandList_->DrawIndexedInstanced(UINT(modelData.mesh.vertexSize), 1, 0, 0, 0);

}

void Draw::DrawAnimation(CharacterAnimator* obj)
{
	preDraw(obj->GetShader(), obj->GetBlend());

	const ModelData& modelData = ModelManager::GetModelData(obj->GetModelNumber());

	// インデックス設定
	commandList_->IASetIndexBuffer(&modelData.mesh.indexBufferView);

	// VBV を複数渡す（頂点データ + インフルエンスデータ）
	D3D12_VERTEX_BUFFER_VIEW vbvs[2];
	vbvs[0] = modelData.mesh.vertexBufferView;
	vbvs[1] = *obj->GetInfluenceBufferView();
	commandList_->IASetVertexBuffers(0, 2, vbvs);



	// ルートをアニメーション用レイアウトに合わせて設定
	commandList_->SetGraphicsRootConstantBufferView(0, obj->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress()); // material (pixel b0)
	commandList_->SetGraphicsRootConstantBufferView(1, obj->GetWvpDataResource()->GetGPUVirtualAddress()); // wvp (vertex b0)
	commandList_->SetGraphicsRootDescriptorTable(2, obj->GetPaletteSrvHandleGPU()); // matrix palette SRV (t0)
	commandList_->SetGraphicsRootDescriptorTable(3, obj->GetTextureSrvHandleGPU()); // texture SRV (t1)
	commandList_->SetGraphicsRootConstantBufferView(4, camera->GetCameraResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(7, spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());

	commandList_->DrawIndexedInstanced(modelData.mesh.indexSize, 1, 0, 0, 0);
}


void Draw::DrawModel(Model* model, Camera* camera)
{
	preDraw(model->GetShader(), model->GetBlend());

	const ModelData& modelData = ModelManager::GetModelData(model->GetModelNumber());
	

	//objectの描画
	commandList_->IASetVertexBuffers(0, 1, &modelData.mesh.vertexBufferView);  // アドレスを渡す
	commandList_->SetGraphicsRootConstantBufferView(0, model->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, model->GetWvpDataResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, model->GetTextureSrvHandleGPU());
	commandList_->SetGraphicsRootConstantBufferView(3, camera->GetCameraResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());

	commandList_->DrawInstanced(UINT(modelData.mesh.vertexSize), 1, 0, 0);
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
	commandList_->SetGraphicsRootConstantBufferView(0, particle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(1, particle->GetInstancingSrvHandleGPU());
	commandList_->SetGraphicsRootDescriptorTable(2, particle->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(6, instanceCount, 0, 0);

}

void Draw::DrawSprite(Sprite* sprite, Camera* camera)
{
	preDraw(sprite->GetShader(), sprite->GetBlend());

	commandList_->IASetIndexBuffer(sprite->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sprite->GetVertexBufferView());//VBVを設定
	commandList_->SetGraphicsRootConstantBufferView(0, sprite->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, sprite->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootDescriptorTable(2, sprite->GetTextureSrvHandleGPU());
	commandList_->SetGraphicsRootConstantBufferView(3, camera->GetCameraResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(4, directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(5, pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(6, spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());

	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
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

