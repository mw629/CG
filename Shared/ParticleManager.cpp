#include "ParticleManager.h"

#include <imgui.h>


void ParticleManager::ImGui() {

	if (ImGui::CollapsingHeader("Particle")) {
		int i = 0;
		for (auto it = particleData_.begin(); it != particleData_.end(); ++it, ++i) {

			// 折りたたみ
			std::string label = "Particle " + std::to_string(i);
			if (ImGui::CollapsingHeader(label.c_str())) {

				ParticleData& p = *it;

				// Transform
				ImGui::Text("Transform");

				ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(),
					&p.transform.translate.x, 0.01f);

				ImGui::DragFloat3(("Rotation##" + std::to_string(i)).c_str(),
					&p.transform.rotate.x, 0.01f);

				ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(),
					&p.transform.scale.x, 0.01f);

				// Color
				ImGui::ColorEdit4(("Color##" + std::to_string(i)).c_str(),
					&p.color.x);
			}
		}
	}
	
	if (ImGui::Button("Add Particle")) {
		Emit();
	}

}

void ParticleManager::Initialize() {


	randomEngine.seed(seedGenerator_());

	particle_.get()->Initialize();

}

void ParticleManager::Update(Matrix4x4 viewMatrix) {

	particle_.get()->Updata(viewMatrix, particleData_);
}

void ParticleManager::Draw() {
	Draw::DrawParticle(particle_.get());
}

ParticleData ParticleManager::MakeNewParticle()
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	ParticleData data;
	data.transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	data.transform.translate = { distribution(randomEngine),distribution(randomEngine) ,distribution(randomEngine) };
	data.velocity = { distribution(randomEngine) ,distribution(randomEngine) ,distribution(randomEngine) };
	data.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 色を初期化
	data.lifeTime = 3.0f; // ライフタイムを設定
	data.currentTime = 0.0f;
	return data;
}

void ParticleManager::Emit()
{
	for (uint32_t count = 0; count < emitter_.count; ++count) {
		particleData_.push_back(MakeNewParticle());
	}
}

