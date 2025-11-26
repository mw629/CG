#pragma once
#include <random>
#include <Engine.h>

struct Emitter {
	Transform transform = { {1.0f,1.0f,1.0f} ,{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };//エミッタのTransform
	uint32_t count = 10;//発生数
	float frequency;//発生頻度
	float frequencyTime;//頻度用時刻
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

	void Update(Matrix4x4 viewMatrix);

	void Draw();
	
	ParticleData MakeNewParticle();

	void Emit();

};

