#pragma once
#include "Engine.h"
#include <random>

class Emitter
{
private:

	Particle particle_;
	std::vector<ParticleData> particleData_;

	int ParticleMaxNum_ = 0;

public:

	void ImGui();

	void Initialize(int particleNum);

	void Update();

	void MakeNewParticle(std::mt19937& random);


};

