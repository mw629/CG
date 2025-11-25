#include "Emitter.h"


void Emitter::ImGui() {

}

void Emitter::Initialize(int particleNum) {

	ParticleMaxNum_ = particleNum;

	for (int i = 0; i < ParticleMaxNum_; i++) {
		ParticleData data;
		data.transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
		data.color = { 1.0f,1.0f,1.0f,1.0f };
		particleData_.push_back(data);
	}
	particle_.Initialize(particleData_);

}

void Emitter::Update() {

}

void Emitter::MakeNewParticle(std::mt19937& random)
{


}

