#include "Draw.h"
#include "Graphics/GraphicsDevice.h"
#include <cassert>
#include "ModelManager.h"
#include "PostEffect.h"
#include "Texture.h"
#include "Graphics/GpuProfiler.h"

void Draw::SetCBV(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS address) {
	UINT index = graphicsPipelineState_->GetRootParameterIndex(shader, blend, name);
	if (index != static_cast<UINT>(-1)) commandList_->SetGraphicsRootConstantBufferView(index, address);
}

void Draw::SetSRV(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS address) {
	UINT index = graphicsPipelineState_->GetRootParameterIndex(shader, blend, name);
	if (index != static_cast<UINT>(-1)) commandList_->SetGraphicsRootShaderResourceView(index, address);
}

void Draw::SetTable(ShaderName shader, BlendMode blend, const std::string& name, D3D12_GPU_DESCRIPTOR_HANDLE handle) {
	D3D12_GPU_DESCRIPTOR_HANDLE useHandle = handle;
	if (useHandle.ptr == 0) {
		Texture tex;
		useHandle = tex.TextureData(0);
	}
	if (useHandle.ptr == 0) return;
	UINT index = graphicsPipelineState_->GetRootParameterIndex(shader, blend, name);
	if (index != static_cast<UINT>(-1)) commandList_->SetGraphicsRootDescriptorTable(index, useHandle);
}

void Draw::Initialize(ID3D12GraphicsCommandList* commandList, GraphicsPipelineState* graphicsPipelineState,
	LightManager* lightManager, LineRenderer* lineRenderer)
{
	commandList_ = commandList;
	graphicsPipelineState_ = graphicsPipelineState;
	lightManager_ = lightManager;
	lineRenderer_ = lineRenderer;
}

void Draw::SetCamera(Camera* setCamera)
{
	camera_ = setCamera;
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
	SetSRV(shader, blend, "gTransformationMatrix", obj->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", obj->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera_->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawIndexedInstanced(UINT(mesh.indexBufferView_.SizeInBytes / sizeof(uint32_t)), obj->GetInstanceCount(), 0, 0, 0);

}

void Draw::DrawAnimation(CharacterAnimator* obj)
{
	ShaderName shader = ObjectShader;
	BlendMode blend = obj->GetBlend();

	ComputePipeline* cp = graphicsPipelineState_->GetComputePipeline();
	if (cp) {
		commandList_->SetComputeRootSignature(cp->GetRootSignature());
		commandList_->SetPipelineState(cp->GetPipelineState());

		UINT paramPalette = cp->GetRootParameterIndex("gMatrixPalette");
		UINT paramInput = cp->GetRootParameterIndex("gInputVertices");
		UINT paramInfluences = cp->GetRootParameterIndex("gInfluences");
		UINT paramOutput = cp->GetRootParameterIndex("gOutputVertices");
		UINT paramInfo = cp->GetRootParameterIndex("gSkinningInformation");

		auto modelData = obj->GetModelData();
		for (size_t i = 0; i < modelData.subMeshes.size(); ++i) {
			const auto& subMesh = modelData.subMeshes[i];

			// Transition to UAV
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Transition.pResource = obj->GetSubMeshSkinnedResource(i);
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			commandList_->ResourceBarrier(1, &barrier);

			// Bind compute root descriptors/tables
			if (paramPalette != static_cast<UINT>(-1)) commandList_->SetComputeRootDescriptorTable(paramPalette, obj->GetPaletteSrvHandleGPU());
			if (paramInput != static_cast<UINT>(-1)) commandList_->SetComputeRootDescriptorTable(paramInput, obj->GetSubMeshInputVertexSrvHandle(i));
			if (paramInfluences != static_cast<UINT>(-1)) commandList_->SetComputeRootDescriptorTable(paramInfluences, obj->GetSubMeshInfluenceSrvHandle(i));
			if (paramOutput != static_cast<UINT>(-1)) commandList_->SetComputeRootDescriptorTable(paramOutput, obj->GetSubMeshOutputVertexUavHandle(i));
			if (paramInfo != static_cast<UINT>(-1)) commandList_->SetComputeRootConstantBufferView(paramInfo, obj->GetSubMeshSkinningInfoResource(i)->GetGPUVirtualAddress());

			if (gpuProfiler_) gpuProfiler_->BeginProfile(commandList_, "Skinning.CS");
			commandList_->Dispatch((static_cast<UINT>(subMesh.mesh.vertexSize) + 1023) / 1024, 1, 1);
			if (gpuProfiler_) gpuProfiler_->EndProfile(commandList_, "Skinning.CS");

			// Transition back to vertex buffer
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			commandList_->ResourceBarrier(1, &barrier);
		}
	}

	preDraw(shader, blend);

	// 共通の設定
	SetSRV(shader, blend, "gTransformationMatrix", obj->GetWvpDataResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gCamera", camera_->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	auto& subMeshMaterials = obj->GetSubMeshMaterials();
	auto modelData = obj->GetModelData();

	for (size_t i = 0; i < modelData.subMeshes.size(); ++i) {
		const auto& subMesh = modelData.subMeshes[i];
		Mesh mesh = subMesh.mesh;

		commandList_->IASetIndexBuffer(&mesh.indexBufferView_);

		// Bind skinned VB view instead of the original unskinned + influence vbvs
		D3D12_VERTEX_BUFFER_VIEW vbv = *obj->GetSubMeshSkinnedBufferView(i);
		commandList_->IASetVertexBuffers(0, 1, &vbv);

		if (i < subMeshMaterials.size()) {
			SetCBV(shader, blend, "gMaterial", subMeshMaterials[i].materialFactory->GetMaterialResource()->GetGPUVirtualAddress());
			SetTable(shader, blend, "gTexture", subMeshMaterials[i].textureSrvHandleGPU);
		} else {
			SetCBV(shader, blend, "gMaterial", obj->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
			SetTable(shader, blend, "gTexture", obj->GetTextureSrvHandleGPU());
		}

		commandList_->DrawIndexedInstanced(UINT(mesh.indexBufferView_.SizeInBytes / sizeof(uint32_t)), obj->GetInstanceCount(), 0, 0, 0);
	}

	if (obj->GetVisibleBones()) {
		DrawAllLines(obj->GetBoneRenderer(), false); // 深度テストなしで手前に表示
		const auto& skeleton = obj->GetSkeleton();
		for (int i = 0; i < skeleton.joints.size(); ++i) {
			if (auto sphere = obj->GetJointSphere(i)) {
				DrawSphere(sphere.get());
			}
		}
	}
}


void Draw::DrawModel(Model* model)
{
	preDraw(model->GetShader(), model->GetBlend());

	ShaderName shader = model->GetShader();
	BlendMode blend = model->GetBlend();

	SetSRV(shader, blend, "gTransformationMatrix", model->GetWvpDataResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gCamera", camera_->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	auto& subMeshMaterials = model->GetSubMeshMaterials();
	auto modelData = ModelManager::GetModelData(model->GetModelNumber());

	for (size_t i = 0; i < modelData.subMeshes.size(); ++i) {
		const auto& subMesh = modelData.subMeshes[i];
		Mesh mesh = subMesh.mesh;

		commandList_->IASetIndexBuffer(&mesh.indexBufferView_);
		commandList_->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);

		if (i < subMeshMaterials.size()) {
			SetCBV(shader, blend, "gMaterial", subMeshMaterials[i].materialFactory->GetMaterialResource()->GetGPUVirtualAddress());
			SetTable(shader, blend, "gTexture", subMeshMaterials[i].textureSrvHandleGPU);
		} else {
			SetCBV(shader, blend, "gMaterial", model->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
			SetTable(shader, blend, "gTexture", model->GetTextureSrvHandleGPU());
		}

		commandList_->DrawIndexedInstanced(UINT(mesh.indexBufferView_.SizeInBytes / sizeof(uint32_t)), model->GetInstanceCount(), 0, 0, 0);
	}
}

void Draw::DrawParticle(EffectDefinition* particle)
{
	if (!particle) return;

	// GPU Particle モードが有効な場合
	if (particle->GetUseGpuParticle()) {
		if (!particle->IsGpuInitialized()) {
			particle->InitializeGPUParticle(commandList_, graphicsPipelineState_->GetComputePipeline());
		}

		// GPU Compute Shader Dispatch (EmitParticle -> UpdateParticle)
		particle->DispatchGPUParticle(commandList_, graphicsPipelineState_->GetComputePipeline());

		preDraw(particle->GetShader(), particle->GetBlend());

		commandList_->IASetVertexBuffers(0, 1, particle->GetVertexBufferView());
		ShaderName shader = particle->GetShader();
		BlendMode blend = particle->GetBlend();
		SetCBV(shader, blend, "gMaterial", particle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
		SetCBV(shader, blend, "gPerView", particle->GetPerViewResource()->GetGPUVirtualAddress());
		SetTable(shader, blend, "gTexture", particle->GetTextureSrvHandleGPU());

		if (particle->GetGpuParticleResource()) {
			SetSRV(shader, blend, "gParticle", particle->GetGpuParticleResource()->GetGPUVirtualAddress());
			commandList_->DrawInstanced(particle->GetVertexSize(), 10000, 0, 0);
		}
		return;
	}

	// 従来の CPU Particle 描画
	const UINT instanceCount = static_cast<UINT>(particle->GetEffectDefinitionNum());
	if (instanceCount == 0) {
		return;
	}

	preDraw(particle->GetShader(), particle->GetBlend());

	commandList_->IASetVertexBuffers(0, 1, particle->GetVertexBufferView());
	ShaderName shader = particle->GetShader();
	BlendMode blend = particle->GetBlend();
	SetCBV(shader, blend, "gMaterial", particle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPerView", particle->GetPerViewResource()->GetGPUVirtualAddress());
	SetSRV(shader, blend, "gParticle", particle->GetInstancingResource()->GetGPUVirtualAddress());
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
	SetSRV(shader, blend, "gTransformationMatrix", sprite->GetVertexResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", sprite->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera_->GetCameraResource()->GetGPUVirtualAddress());
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
	SetSRV(shader, blend, "gTransformationMatrix", sphere->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", sphere->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera_->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gDirectionalLightGroup", lightManager_->GetDirectionalLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gPointLightGroup", lightManager_->GetPointLightResource()->GetGPUVirtualAddress());
	SetCBV(shader, blend, "gSpotLightGroup", lightManager_->GetSpotLightResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawInstanced(static_cast<UINT>(pow(sphere->GetSubdivision(), 2) * 6), sphere->GetInstanceCount(), 0, 0);
}

void Draw::DrawTriangle(Triangle* triangle)
{
	preDraw(triangle->GetShader(), triangle->GetBlend());

	commandList_->IASetVertexBuffers(0, 1, triangle->GetVertexBufferView());//VBVを設定
	ShaderName shader = triangle->GetShader();
	BlendMode blend = triangle->GetBlend();
	SetCBV(shader, blend, "gMaterial", triangle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetSRV(shader, blend, "gTransformationMatrix", triangle->GetVertexResource()->GetGPUVirtualAddress());
	SetTable(shader, blend, "gTexture", triangle->GetTextureSrvHandleGPU());
	SetCBV(shader, blend, "gCamera", camera_->GetCameraResource()->GetGPUVirtualAddress());
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

void Draw::DrawAllLines(LineRenderer* lineRenderer, bool depthTest)
{
	if (!lineRenderer) return;
	ShaderName shader = depthTest ? "LineShader" : "LineShaderNoDepth";
	preDraw(shader, kBlendModeNormal);
	SetCBV(shader, kBlendModeNormal, "gTransform", lineRenderer->GetWVPResource()->GetGPUVirtualAddress());
	lineRenderer->DrawAll(commandList_, camera_);
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

		// Pixelate 用定数バッファ (register b1) をバインド
		UINT pixelCbIndex = graphicsPipelineState_->GetRootParameterIndex(shader, BlendMode::kBlendModeNone, "PixelationParams");
		if (pixelCbIndex != static_cast<UINT>(-1) && postEffect->GetPixelationBufferResource()) {
			commandList_->SetGraphicsRootConstantBufferView(pixelCbIndex, postEffect->GetPixelationBufferResource()->GetGPUVirtualAddress());
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



