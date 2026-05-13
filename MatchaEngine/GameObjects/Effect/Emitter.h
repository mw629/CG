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

class Emitter
{
private:

	std::unique_ptr<EffectDefinition> effectDefinition_ = std::make_unique<EffectDefinition>();
	std::list<EffectDefinitionData> effectDefinitionData_;

	AccelerationFiled accelerationFiled_;

	EffectDefinitionData SetEffectDefinitionData_;

	EmitterData emitter_;
	std::mt19937 randomEngine;

	std::random_device seedGenerator_;

	
	bool isStop_=false;
	bool isHit_ = false;


public:

	// 生成時の振る舞いを注入する関数
	std::function<void(EffectDefinitionData&)> generatorBehavior = nullptr;

	void ImGui();

	void Initialize();

	void Initialize(EmitterData emitter);
	void Initialize(EmitterData emitter,EffectDefinitionData particleData);
	void Initialize(EmitterData emitter, EffectDefinitionData particleData,int TextureHandle);

	void Update(Matrix4x4 viewMatrix);
	void Update(Matrix4x4 viewMatrix, std::function<EffectDefinitionData(const EffectDefinitionData&)> moveBehavior);//動きに変化をつけたい場合
	void Update(EmitterData emitter, Matrix4x4 viewMatrix, std::function<EffectDefinitionData(const EffectDefinitionData&)> moveBehavior);//動きに変化をつけたい場合
	void Update(Matrix4x4 viewMatrix, Vector3 scale);

	void Draw();
	
	EffectDefinitionData MakeNewParticle();
	EffectDefinitionData MakeNewParticle(Vector3 scale);//サイズを変えれるnew
 
	EffectDefinitionData particleMove(EffectDefinitionData p);
	EffectDefinitionData particleMoveFire(EffectDefinitionData p);
	void EmitSize();

	bool OnCollision(EffectDefinitionData particleData);

	void Emit();

	void SetAccelerationFiled(AccelerationFiled accelerationFiled) { accelerationFiled_ = accelerationFiled; }
	void SetBlend(BlendMode blend) { effectDefinition_.get()->SetBlend(blend); }



	std::list<EffectDefinitionData> GetEffectDefinitionData() { return effectDefinition_.get()->GetEffectDefinitionData(); }
};

