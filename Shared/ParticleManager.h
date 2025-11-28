#pragma once
#include <random>
#include <Engine.h>

struct Emitter {
	Transform transform = { {1.0f,1.0f,1.0f} ,{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };//エミッタのTransform
	uint32_t count = 10;//発生数
	float frequency=0.5f;//発生頻度
	float frequencyTime=0.0f;//頻度用時刻
};


class ParticleManager
{
private:

	std::unique_ptr<Particle> particle_ = std::make_unique<Particle>();
	std::list<ParticleData> particleData_;

	Emitter emitter_;
	std::mt19937 randomEngine;


	std::random_device seedGenerator_;

	



public:

	void ImGui();

	void Initialize();

	void Initialize(Emitter emitter);

	void Update(Matrix4x4 viewMatrix);
	void Update(Matrix4x4 viewMatrix, ParticleData particleMove(ParticleData particleData));//動きに変化をつけたい場合

	void Update(Matrix4x4 viewMatrix, Vector3 scale);

	void Draw();
	
	ParticleData MakeNewParticle();
	ParticleData MakeNewParticle(Vector3 scale);//サイズを変えれるnew
 
	void Emit();


};

