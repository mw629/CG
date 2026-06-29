#pragma once
#include <random>
#include <functional>
#include "EffectDefinition.h"

struct EmitterData {
	Transform transform = { {1.0f,1.0f,1.0f} ,{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };//エミッタのTransform
	uint32_t count = 10;//発生数
	float frequency=0.5f;//発生頻度
	float frequencyTime=0.0f;//頻度用時刻
};

struct AccelerationFiled {
	Vector3 acceleration = { 0.05f,0.0f,0.0f };
	AABB area = { { -10.0f,-10.0f,-10.0f },{10.0f,10.0f,10.0f} };
};

struct ParticleMovementData {
	Vector3 baseVelocity = { 0.0f, 0.0f, 0.0f };
	Vector3 velocityVariance = { 1.0f / 60.0f, 1.0f / 60.0f, 1.0f / 60.0f };
	Vector3 acceleration = { 0.0f, 0.0f, 0.0f };
	Vector3 sizeVariance = { 0.0f, 0.0f, 0.0f };
	Vector3 sizeDelta = { 1.0f, 1.0f, 1.0f }; // Multiplied each frame
};

class Emitter
{
private:

	std::unique_ptr<EffectDefinition> effectDefinition_ = std::make_unique<EffectDefinition>();
	std::list<EffectDefinitionData> effectDefinitionData_;

	AccelerationFiled accelerationFiled_;

	EffectDefinitionData SetEffectDefinitionData_;
	ParticleMovementData movementData_;

	std::string texturePath_ = "Resources/Texture/circle.png";
	EffectShape shape_ = EffectShape::Plane;
	EffectShapeData shapeData_;

	EmitterData emitter_;
	std::mt19937 randomEngine;

	ShaderName shaderName_ = "ParticleShader";

	std::random_device seedGenerator_;

	
	bool isStop_=false;
	bool isHit_ = false;


public:
	std::string name_ = "Particle";
	char saveFileName_[256] = "particle";

	// 生成時の振る舞いを注入する関数
	std::function<void(EffectDefinitionData&)> generatorBehavior = nullptr;

	void SetEmitterData(const EmitterData& data) { emitter_ = data; }
	EmitterData GetEmitterData() const { return emitter_; }

	void SetStop(bool isStop) { isStop_ = isStop; }
	void ClearParticles() { effectDefinitionData_.clear(); }

	void ImGui();

	void SaveToJson(const std::string& name);
	void LoadFromJson(const std::string& name);

	void Initialize(EffectShape shape = EffectShape::Plane);

	void Initialize(EmitterData emitter, EffectShape shape = EffectShape::Plane);
	void Initialize(EmitterData emitter,EffectDefinitionData particleData, EffectShape shape = EffectShape::Plane);
	void Initialize(EmitterData emitter, EffectDefinitionData particleData,int TextureHandle, EffectShape shape = EffectShape::Plane);

	void Update(Matrix4x4 viewMatrix);
	void Update(Matrix4x4 viewMatrix, std::function<EffectDefinitionData(const EffectDefinitionData&)> moveBehavior);//動きに変化をつけたい場合
	void Update(EmitterData emitter, Matrix4x4 viewMatrix, std::function<EffectDefinitionData(const EffectDefinitionData&)> moveBehavior);//動きに変化をつけたい場合
	void Update(Matrix4x4 viewMatrix, Vector3 scale);

	void EditorUpdate(Matrix4x4 viewMatrix);

	void Draw();
	
	EffectDefinitionData MakeNewParticle();
	EffectDefinitionData MakeNewParticle(Vector3 scale);//サイズを変えれるnew
 
	EffectDefinitionData particleMove(EffectDefinitionData p);
	EffectDefinitionData particleMoveFire(EffectDefinitionData p);
	void EmitSize();

	bool OnCollision(EffectDefinitionData particleData);

	void Emit();

	void SetAccelerationFiled(AccelerationFiled accelerationFiled) { accelerationFiled_ = accelerationFiled; }
	void DeleteParticle(int ParticleNum) { effectDefinition_.get()->DeleteParticle(ParticleNum); }

	void SetTexturePath(const std::string& path);
	void SetShape(EffectShape shape);
	void SetShapeData(const EffectShapeData& data);
	std::string GetTexturePath() const { return texturePath_; }
	EffectShape GetShape() const { return shape_; }
	EffectShapeData GetShapeData() const { return shapeData_; }
	void SetBlend(BlendMode blend) { effectDefinition_.get()->SetBlend(blend); }
	BlendMode GetBlend() const { return effectDefinition_.get()->GetBlend(); }

	void SetShader(ShaderName shader) { shaderName_ = shader; effectDefinition_->SetShader(shader); }
	ShaderName GetShader() const { return shaderName_; }

	void SetBillboard(bool flag) { effectDefinition_->SetBillboard(flag); }
	bool GetBillboard() const { return effectDefinition_->GetBillboard(); }



	std::list<EffectDefinitionData> GetEffectDefinitionData() { return effectDefinition_.get()->GetEffectDefinitionData(); }
};
