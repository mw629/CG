#include "ParticleManager.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#include <sstream>
#endif // _USE_IMGUI




void ParticleManager::ImGui() {
#ifdef _USE_IMGUI
	// インスタンス毎にユニークなウィンドウIDを付与（表示名は"Particle"のまま）
	std::ostringstream oss;
	oss << "Particle###" << static_cast<const void*>(this);
	const std::string windowTitle = oss.str();

	if (ImGui::Begin(windowTitle.c_str())) {

		// このインスタンス専用のIDスコープ
		ImGui::PushID(this);

		if (ImGui::CollapsingHeader("Particle")) {
			int i = 0;
			for (auto it = particleData_.begin(); it != particleData_.end(); ++it, ++i) {

				// 折りたたみ
				std::string label = "Particle " + std::to_string(i);
				if (ImGui::CollapsingHeader(label.c_str())) {

					ParticleData& p = *it;

					// 個別要素もIDを安定化
					ImGui::PushID(i);

					// Transform
					ImGui::Text("Transform");
					ImGui::DragFloat3("Position", &p.transform.translate.x, 0.01f);
					ImGui::DragFloat3("Rotation", &p.transform.rotate.x, 0.01f);
					ImGui::DragFloat3("Scale", &p.transform.scale.x, 0.01f);

					// Color
					ImGui::ColorEdit4("Color", &p.color.x);

					ImGui::PopID();
				}
			}
		}

		if (ImGui::CollapsingHeader("Emitter")) {

			if (ImGui::CollapsingHeader("Transform")) {
				ImGui::DragFloat3("Position", &emitter_.transform.translate.x, 0.01f);
				ImGui::DragFloat3("Rotation", &emitter_.transform.rotate.x, 0.01f);
				ImGui::DragFloat3("Scale", &emitter_.transform.scale.x, 0.01f);
			}

			// Count
			ImGui::DragInt("Count", reinterpret_cast<int*>(&emitter_.count), 1, 0, 10000);

			// Frequency
			ImGui::DragFloat("Frequency", &emitter_.frequency, 0.01f, 0.0f, 10.0f);

			// Frequency Time (デバッグ用表示)
			ImGui::DragFloat("Frequency Time", &emitter_.frequencyTime, 0.01f, 0.0f, 9999.0f);
		}

		if (ImGui::Button("Add Particle")) {
			Emit();
		}

		ImGui::PopID(); // this のIDスコープ終了
	}
	ImGui::End(); // Beginがfalseでも必ず対応して呼ぶ
#endif // _USE_IMGUI

}

void ParticleManager::Initialize() {


	randomEngine.seed(seedGenerator_());

	particle_.get()->Initialize();

}

void ParticleManager::Initialize(Emitter emitter)
{
	emitter_ = emitter;

	randomEngine.seed(seedGenerator_());

	particle_.get()->Initialize();
}

void ParticleManager::Update(Matrix4x4 viewMatrix) {

	for (std::list<ParticleData>::iterator particleIterator = particleData_.begin();
		particleIterator != particleData_.end(); ) {

		// 位置を更新
		particleIterator->transform.translate += particleIterator->velocity;

		particleIterator->color.w -= 1.0f / (particleIterator->lifeTime * 60.0f);
		particleIterator->currentTime += 1.0f / 60.0f;

		++particleIterator;//これを忘れた未来の僕がいるならこれを忘れた今の僕が悲しむ
	}

	emitter_.frequencyTime += 1.0f / 60.0f;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		Emit();
		emitter_.frequencyTime -= emitter_.frequency;
	}

	particle_.get()->Updata(viewMatrix, particleData_);
}

void ParticleManager::Update(Matrix4x4 viewMatrix, ParticleData particleMove(ParticleData particleData))
{
	int i = 0;
	for (std::list<ParticleData>::iterator particleIterator = particleData_.begin();
		particleIterator != particleData_.end(); ) {

		ParticleData particleData = particleMove(*particleIterator);

		particleIterator->velocity = particleData.velocity;
		particleIterator->transform.translate += particleIterator->velocity;
		particleIterator->transform.scale = particleData.transform.scale;
		particleIterator->transform.rotate = particleData.transform.rotate;
		particleIterator->color.w -= 1.0f / (particleIterator->lifeTime * 60.0f);
		particleIterator->currentTime += 1.0f / 60.0f;

		++particleIterator;//これを忘れた未来の僕がいるならこれを忘れた今の僕が悲しむ
		++i;
	}

	emitter_.frequencyTime += 1.0f / 60.0f;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		Emit();
		emitter_.frequencyTime -= emitter_.frequency;
	}

	particle_.get()->Updata(viewMatrix, particleData_);
}

void ParticleManager::Update(Matrix4x4 viewMatrix, Vector3 scale)
{
	for (std::list<ParticleData>::iterator particleIterator = particleData_.begin();
		particleIterator != particleData_.end(); ) {

		// 位置を更新
		particleIterator->transform.translate += particleIterator->velocity;

		particleIterator->color.w -= 1.0f / (particleIterator->lifeTime * 60.0f);
		particleIterator->currentTime += 1.0f / 60.0f;

		++particleIterator;//これを忘れた未来の僕がいるならこれを忘れた今の僕が悲しむ
	}

	emitter_.frequencyTime += 1.0f / 60.0f;
	if (emitter_.frequency <= emitter_.frequencyTime) {
		Emit();
		emitter_.frequencyTime -= emitter_.frequency;
	}

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
	Vector3 possion = { distribution(randomEngine),distribution(randomEngine) ,distribution(randomEngine) };
	data.transform.translate = possion * emitter_.transform.scale + emitter_.transform.translate;
	data.velocity = { distribution(randomEngine)/60.0f ,distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f };
	data.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 色を初期化
	data.lifeTime = 3.0f; // ライフタイムを設定
	data.currentTime = 0.0f;
	return data;
}

ParticleData ParticleManager::MakeNewParticle(Vector3 scale)
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	ParticleData data;
	data.transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Vector3 possion = { distribution(randomEngine),distribution(randomEngine) ,distribution(randomEngine) };
	data.transform.translate = possion * emitter_.transform.scale + emitter_.transform.translate;
	data.transform.scale = scale;
	data.velocity = { distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f };
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

