#include "Draw.h"
#include "Graphics/GraphicsDevice.h"
#include <cassert>
#include "ModelManager.h"
#include "PostEffect.h"
#include "Texture.h"

namespace {
	ID3D12GraphicsCommandList* commandList_{};
	GraphicsPipelineState* graphicsPipelineState_;
	LightManager* lightManager_;
	Camera* camera;
	D3D12_GPU_DESCRIPTOR_HANDLE environmentTextureSrvHandleGPU_{};

	void SetCBV(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS address) {
		UINT index = graphicsPipelineState_->GetRootParameterIndex(shader, blend, name);
		if (index != static_cast<UINT>(-1)) commandList_->SetGraphicsRootConstantBufferView(index, address);
	}

	void SetTable(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_DESCRIPTOR_HANDLE handle) {
		D3D12_GPU_DESCRIPTOR_HANDLE useHandle = handle;
		if (useHandle.ptr == 0) {
			Texture tex;
			useHandle = tex.TextureData(0);
		}
		if (useHandle.ptr == 0) return;
		UINT index = graphicsPipelineState_->GetRootParameterIndex(shader, blend, name);
		if (index != static_cast<UINT>(-1)) commandList_->SetGraphicsRootDescriptorTable(index, useHandle);
	}
}

void Draw::Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState,
	LightManager* lightManager)
{
	commandList_ = commandList;
	graphicsPipelineState_ = graphicsPipelineState;
	lightManager_ = lightManager;
}

void Draw::SetCamera(Camera* setCamera)
{
	camera = setCamera;
}

void Draw::SetEnvironmentTexture(int handle)
{
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();
	environmentTextureSrvHandleGPU_ = texture_.get()->TextureData(handle);
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

	Mesh mesh = obj->GetMesh();


	//objectの描画
	commandList_->IASetIndexBuffer(&mesh.indexBufferView_);
	commandList_->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);  
	
	ShaderName shader = obj->GetShader();
	BlendMode blend = obj->GetBlend();
	SetCBV(shader, blend, "gMaterial", obj->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gTransformationMatrix", obj->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", obj->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawIndexedInstanced(UINT(mesh.indexBufferView_.SizeInBytes / sizeof(uint32_t)), 1, 0, 0, 0);

}

void Draw::DrawAnimation(CharacterAnimator* obj)
{
	preDraw(obj->GetShader(), obj->GetBlend());

	Mesh mesh = obj->GetMesh();

	// インデックス設定
	commandList_->IASetIndexBuffer(&mesh.indexBufferView_);

	// VBV を複数渡す（頂点データ + インフルエンスデータ）
	D3D12_VERTEX_BUFFER_VIEW vbvs[2];
	vbvs[0] = mesh.vertexBufferView;
	vbvs[1] = *obj->GetInfluenceBufferView();
	commandList_->IASetVertexBuffers(0, 2, vbvs);



	// ルートをアニメーション用レイアウトに合わせて設定
	ShaderName shader = obj->GetShader();
	BlendMode blend = obj->GetBlend();
	SetCBV(shader, blend, "gMaterial", obj->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gTransformationMatrix", obj->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gMatrixPalette", obj->GetPaletteSrvHandleGPU());
	SetTable(shader, blend, "gTexture", obj->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());

	commandList_->DrawIndexedInstanced(UINT(mesh.indexBufferView_.SizeInBytes / sizeof(uint32_t)), 1, 0, 0, 0);
}


void Draw::DrawModel(Model* model)
{
	preDraw(model->GetShader(), model->GetBlend());

	Mesh mesh = ModelManager::GetModelData(model->GetModelNumber()).mesh;


	//objectの描画
	commandList_->IASetIndexBuffer(&mesh.indexBufferView_);
	commandList_->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);  // アドレスを渡す
	ShaderName shader = model->GetShader();
	BlendMode blend = model->GetBlend();
	SetCBV(shader, blend, "gMaterial", model->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gTransformationMatrix", model->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", model->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawIndexedInstanced(UINT(mesh.indexBufferView_.SizeInBytes / sizeof(uint32_t)), 1, 0, 0, 0);
}

void Draw::DrawParticle(EffectDefinition* particle)
{
	// インスタンス数が0なら描画しない
   const UINT instanceCount = static_cast<UINT>(particle->GetEffectDefinitionNum());
	if (instanceCount == 0) {
		return;
	}

	preDraw(particle->GetShader(), particle->GetBlend());

	commandList_->IASetVertexBuffers(0, 1, particle->GetVertexBufferView());//VBVを設定
	ShaderName shader = particle->GetShader();
	BlendMode blend = particle->GetBlend();
	SetCBV(shader, blend, "gMaterial", particle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gParticle", particle->GetInstancingSrvHandleGPU());
	SetTable(shader, blend, "gTexture", particle->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(particle->GetVertexSize(), instanceCount, 0, 0);

}

void Draw::DrawSprite(Sprite* sprite)
{
	preDraw(sprite->GetShader(), sprite->GetBlend());
	ShaderName shader = sprite->GetShader();
	BlendMode blend = sprite->GetBlend();

	commandList_->IASetIndexBuffer(sprite->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sprite->GetVertexBufferView());//VBVを設定
	
	SetCBV(shader, blend, "gMaterial", sprite->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gTransformationMatrix", sprite->GetVertexResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", sprite->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Draw::DrawSphere(Sphere* sphere)
{

	preDraw(sphere->GetShader(), sphere->GetBlend());


	//commandList_->IASetIndexBuffer(sphere->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sphere->GetVertexBufferView());//VBVを設定
	ShaderName shader = sphere->GetShader();
	BlendMode blend = sphere->GetBlend();
	SetCBV(shader, blend, "gMaterial", sphere->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gTransformationMatrix", sphere->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", sphere->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawInstanced(static_cast<UINT>(pow(sphere->GetSubdivision(), 2) * 6), 1, 0, 0);
}

void Draw::DrawTriangle(Triangle* triangle)
{
	preDraw(triangle->GetShader(), triangle->GetBlend());

	commandList_->IASetVertexBuffers(0, 1, triangle->GetVertexBufferView());//VBVを設定
	ShaderName shader = triangle->GetShader();
	BlendMode blend = triangle->GetBlend();
	SetCBV(shader, blend, "gMaterial", triangle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gTransformationMatrix", triangle->GetVertexResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", triangle->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);


	commandList_->DrawInstanced(3, 1, 0, 0);
}



void Draw::DrawLine(Line* line)
{
	preDraw("LineShader", kBlendModeNormal);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList_->IASetVertexBuffers(0, 1, line->GetVertexBufferView());//VBVを設定
	SetCBV("LineShader", kBlendModeNormal, "gTransform", line->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->DrawInstanced(2, 1, 0, 0);
}

void Draw::DrawGrid(Grid* grid)
{
	preDraw("LineShader", kBlendModeNormal);
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList_->IASetVertexBuffers(0, 1, grid->GetVertexBufferView());//VBVを設定
	SetCBV("LineShader", kBlendModeNormal, "gTransform", grid->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->DrawInstanced(grid->GetSubdivision() * 4, 1, 0, 0);
}

void Draw::DrawPostEffect(D3D12_GPU_DESCRIPTOR_HANDLE textureHandle, ShaderName shader, PostEffect* postEffect, D3D12_GPU_DESCRIPTOR_HANDLE depthTextureHandle)
{
	preDraw(shader, BlendMode::kBlendModeNone);
	
	// Bind main screen texture using reflection name "gTexture" if present, otherwise fallback to slot 0
	UINT gTexIndex = graphicsPipelineState_->GetRootParameterIndex(shader, BlendMode::kBlendModeNone, "gTexture");
	if (gTexIndex != static_cast<UINT>(-1)) {
		D3D12_GPU_DESCRIPTOR_HANDLE useHandle = textureHandle;
		if (useHandle.ptr == 0) {
			Texture tex;
			useHandle = tex.TextureData(0);
		}
		if (useHandle.ptr != 0) {
			commandList_->SetGraphicsRootDescriptorTable(gTexIndex, useHandle);
		}
	}

	if (depthTextureHandle.ptr != 0) {
		UINT gDepthTexIndex = graphicsPipelineState_->GetRootParameterIndex(shader, BlendMode::kBlendModeNone, "gDepthTexture");
		if (gDepthTexIndex != static_cast<UINT>(-1)) {
			commandList_->SetGraphicsRootDescriptorTable(gDepthTexIndex, depthTextureHandle);
		}
	}

	if (postEffect) {
		// Bind post effect parameters constant buffer
		UINT cbIndex = graphicsPipelineState_->GetRootParameterIndex(shader, BlendMode::kBlendModeNone, "gPostEffect");
		if (cbIndex != static_cast<UINT>(-1) && postEffect->GetConstantBufferResource()) {
			commandList_->SetGraphicsRootConstantBufferView(cbIndex, postEffect->GetConstantBufferResource()->GetGPUVirtualAddress());
		}

		// Bind any additional textures registered in the post effect
		for (const auto& [name, path] : postEffect->GetTexturePaths()) {
			UINT texIndex = graphicsPipelineState_->GetRootParameterIndex(shader, BlendMode::kBlendModeNone, name);
			if (texIndex != static_cast<UINT>(-1)) {
				Texture texture;
				D3D12_GPU_DESCRIPTOR_HANDLE handle = texture.TextureData(path);
				if (handle.ptr == 0) {
					handle = texture.TextureData(0);
				}
				if (handle.ptr != 0) {
					commandList_->SetGraphicsRootDescriptorTable(texIndex, handle);
				}
			}
		}
	}

	commandList_->DrawInstanced(3, 1, 0, 0);
}



