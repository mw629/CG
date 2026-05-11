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
	D3D12_GPU_DESCRIPTOR_HANDLE environmentTextureSrvHandleGPU_{};

	// シェーダーリフレクションで取得した変数名（name）から自動的にRootParameterのインデックスを検索し、CBVをセットするヘルパー関数
	void SetCBV(const std::string& shader, BlendMode blend, const std::string& name, D3D12_GPU_VIRTUAL_ADDRESS address) {
		UINT index = graphicsPipelineState_->GetRootParameterIndex(shader, blend, name);
		// シェーダー内に該当の変数が存在する場合のみ設定を行う（新しくシェーダーを追加・変更しても安全に動作する）
		if (index != static_cast<UINT>(-1)) commandList_->SetGraphicsRootConstantBufferView(index, address);
	}

	// 同様に、変数名からインデックスを検索し、DescriptorTable（SRV, UAVなど）をセットするヘルパー関数
	void SetTable(const std::string& shader, BlendMode blend, const std::string& name, D3D12_GPU_DESCRIPTOR_HANDLE handle) {
		UINT index = graphicsPipelineState_->GetRootParameterIndex(shader, blend, name);
		if (index != static_cast<UINT>(-1)) commandList_->SetGraphicsRootDescriptorTable(index, handle);
	}
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

void Draw::SetEnvironmentTexture(int handle)
{
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();
	environmentTextureSrvHandleGPU_ = texture_.get()->TextureData(handle);
}

void Draw::preDraw(const std::string& shader, BlendMode blend)
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

	SetCBV(obj->GetShader(), obj->GetBlend(), "gMaterial", obj->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gTransformationMatrix", obj->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(obj->GetShader(), obj->GetBlend(), "gTexture", obj->GetTextureSrvHandleGPU());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gDirectionalLight", directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gPointLight", pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gSpotLight", spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetTable(obj->GetShader(), obj->GetBlend(), "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

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
	SetCBV(obj->GetShader(), obj->GetBlend(), "gMaterial", obj->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress()); // material (pixel b0)
	SetCBV(obj->GetShader(), obj->GetBlend(), "gTransformationMatrix", obj->GetWvpDataResource()->GetGPUVirtualAddress()); // wvp (vertex b0)
	SetTable(obj->GetShader(), obj->GetBlend(), "gMatrixPalette", obj->GetPaletteSrvHandleGPU()); // matrix palette SRV (t0)
	SetTable(obj->GetShader(), obj->GetBlend(), "gTexture", obj->GetTextureSrvHandleGPU()); // texture SRV (t1)
	SetCBV(obj->GetShader(), obj->GetBlend(), "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gDirectionalLight", directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gPointLight", pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(obj->GetShader(), obj->GetBlend(), "gSpotLight", spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());

	commandList_->DrawIndexedInstanced(UINT(mesh.indexBufferView_.SizeInBytes / sizeof(uint32_t)), 1, 0, 0, 0);
}


void Draw::DrawModel(Model* model)
{
	preDraw(model->GetShader(), model->GetBlend());

	Mesh mesh = ModelManager::GetModelData(model->GetModelNumber()).mesh;


	//objectの描画
	commandList_->IASetVertexBuffers(0, 1, &mesh.vertexBufferView);  // アドレスを渡す
	SetCBV(model->GetShader(), model->GetBlend(), "gMaterial", model->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(model->GetShader(), model->GetBlend(), "gTransformationMatrix", model->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(model->GetShader(), model->GetBlend(), "gTexture", model->GetTextureSrvHandleGPU());
	SetCBV(model->GetShader(), model->GetBlend(), "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(model->GetShader(), model->GetBlend(), "gDirectionalLight", directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(model->GetShader(), model->GetBlend(), "gPointLight", pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(model->GetShader(), model->GetBlend(), "gSpotLight", spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetTable(model->GetShader(), model->GetBlend(), "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawInstanced(UINT(mesh.vertexSize), 1, 0, 0);
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
	SetCBV(particle->GetShader(), particle->GetBlend(), "gMaterial", particle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetTable(particle->GetShader(), particle->GetBlend(), "gParticle", particle->GetInstancingSrvHandleGPU());
	SetTable(particle->GetShader(), particle->GetBlend(), "gTexture", particle->GetTextureSrvHandleGPU());
	commandList_->DrawInstanced(6, instanceCount, 0, 0);

}

void Draw::DrawSprite(Sprite* sprite)
{
	preDraw(sprite->GetShader(), sprite->GetBlend());

	commandList_->IASetIndexBuffer(sprite->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sprite->GetVertexBufferView());//VBVを設定
	SetCBV(sprite->GetShader(), sprite->GetBlend(), "gMaterial", sprite->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(sprite->GetShader(), sprite->GetBlend(), "gTransformationMatrix", sprite->GetVertexResource()->GetGPUVirtualAddress());
	SetTable(sprite->GetShader(), sprite->GetBlend(), "gTexture", sprite->GetTextureSrvHandleGPU());
	SetCBV(sprite->GetShader(), sprite->GetBlend(), "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(sprite->GetShader(), sprite->GetBlend(), "gDirectionalLight", directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(sprite->GetShader(), sprite->GetBlend(), "gPointLight", pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(sprite->GetShader(), sprite->GetBlend(), "gSpotLight", spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetTable(sprite->GetShader(), sprite->GetBlend(), "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Draw::DrawSphere(Sphere* sphere)
{

	preDraw(sphere->GetShader(), sphere->GetBlend());


	//commandList_->IASetIndexBuffer(sphere->GetIndexBufferView());//IBVを設定
	commandList_->IASetVertexBuffers(0, 1, sphere->GetVertexBufferView());//VBVを設定
	SetCBV(sphere->GetShader(), sphere->GetBlend(), "gMaterial", sphere->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(sphere->GetShader(), sphere->GetBlend(), "gTransformationMatrix", sphere->GetWvpDataResource()->GetGPUVirtualAddress());
	SetTable(sphere->GetShader(), sphere->GetBlend(), "gTexture", sphere->GetTextureSrvHandleGPU());
	SetCBV(sphere->GetShader(), sphere->GetBlend(), "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(sphere->GetShader(), sphere->GetBlend(), "gDirectionalLight", directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(sphere->GetShader(), sphere->GetBlend(), "gPointLight", pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(sphere->GetShader(), sphere->GetBlend(), "gSpotLight", spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetTable(sphere->GetShader(), sphere->GetBlend(), "gEnvironmentTexture", environmentTextureSrvHandleGPU_);

	commandList_->DrawInstanced(static_cast<UINT>(pow(sphere->GetSubdivision(), 2) * 6), 1, 0, 0);
}

void Draw::DrawTriangle(Triangle* triangle)
{
	preDraw(triangle->GetShader(), triangle->GetBlend());

	commandList_->IASetVertexBuffers(0, 1, triangle->GetVertexBufferView());//VBVを設定
	SetCBV(triangle->GetShader(), triangle->GetBlend(), "gMaterial", triangle->GetMartial()->GetMaterialResource()->GetGPUVirtualAddress());
	SetCBV(triangle->GetShader(), triangle->GetBlend(), "gTransformationMatrix", triangle->GetVertexResource()->GetGPUVirtualAddress());
	SetTable(triangle->GetShader(), triangle->GetBlend(), "gTexture", triangle->GetTextureSrvHandleGPU());
	SetCBV(triangle->GetShader(), triangle->GetBlend(), "gCamera", camera->GetCameraResource()->GetGPUVirtualAddress());
	SetCBV(triangle->GetShader(), triangle->GetBlend(), "gDirectionalLight", directionalLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(triangle->GetShader(), triangle->GetBlend(), "gPointLight", pointLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetCBV(triangle->GetShader(), triangle->GetBlend(), "gSpotLight", spotLight_->GetDirectinalLightResource()->GetGPUVirtualAddress());
	SetTable(triangle->GetShader(), triangle->GetBlend(), "gEnvironmentTexture", environmentTextureSrvHandleGPU_);


	commandList_->DrawInstanced(3, 1, 0, 0);
}



void Draw::DrawLine(Line* line)
{
	commandList_->IASetVertexBuffers(0, 1, line->GetVertexBufferView());//VBVを設定
	// シェーダー名を文字列ID（"Line"）で直接指定してリソースをバインドします
	SetCBV("Line", kBlendModeNormal, "gTransformationMatrix", line->GetVertexResource()->GetGPUVirtualAddress());
	commandList_->DrawInstanced(2, 1, 0, 0);
}

void Draw::DrawGrid(Grid* grid)
{
	commandList_->IASetVertexBuffers(0, 1, grid->GetVertexBufferView());//VBVを設定
	// Gridも同様に "Line" シェーダーを使用するように指定しています
	SetCBV("Line", kBlendModeNormal, "gTransformationMatrix", grid->GetVertexResource()->GetGPUVirtualAddress()); // Assuming Grid uses LineShader
	commandList_->DrawInstanced(grid->GetSubdivision() * 4, 1, 0, 0);
}

