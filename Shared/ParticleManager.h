#pragma once
#include <random>
#include <Engine.h>

struct Emitter {
	Transform transform = { {1.0f,1.0f,1.0f} ,{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };//エミッタのTransform
	uint32_t count = 10;//発生数
	float frequency=0.5f;//発生頻度
	float frequencyTime=0.0f;//頻度用時刻
};

struct AccelerationFiled {
	Vector3 acceleration = { 0.05f,0.0f,0.0f };
	AABB area = { { -10.0f,-10.0f,-10.0f },{10.0f,10.0f,10.0f} };
};

class ParticleManager
{
private:

	std::unique_ptr<Particle> particle_ = std::make_unique<Particle>();
	std::list<ParticleData> particleData_;

	AccelerationFiled accelerationFiled_;

	ParticleData SetParticleData_;

	Emitter emitter_;
	std::mt19937 randomEngine;

	std::random_device seedGenerator_;

	
	bool isStop_=false;
	bool isHit_ = false;


public:

	void ImGui();

	void Initialize();

	void Initialize(Emitter emitter);
	void Initialize(Emitter emitter,ParticleData particleData);
	void Initialize(Emitter emitter, ParticleData particleData,int TextureHandle);

	void Update(Matrix4x4 viewMatrix);
	void Update(Matrix4x4 viewMatrix, int a);//動きに変化をつけたい場合
	void Update(Emitter emitter, Matrix4x4 viewMatrix, int a);//動きに変化をつけたい場合
	void Update(Matrix4x4 viewMatrix, Vector3 scale);

	void Draw();
	
	ParticleData MakeNewParticle();
	ParticleData MakeNewParticle(Vector3 scale);//サイズを変えれるnew
 
	ParticleData particleMove(ParticleData p);
	ParticleData particleMoveFire(ParticleData p);

	void EmitSize();

	bool OnCollision(ParticleData particleData);

	void Emit();

	void SetAccelerationFiled(AccelerationFiled accelerationFiled) { accelerationFiled_ = accelerationFiled; }
	void SetBlend(BlendMode blend) { particle_.get()->SetBlend(blend); }

	std::list<ParticleData> GetParticleData() { return particle_.get()->GetParticleData(); }
};

