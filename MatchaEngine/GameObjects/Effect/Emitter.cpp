#include "Emitter.h"
#include <functional>
#include "Input.h"
#include "Calculation.h"
#include "Draw.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#include <sstream>
#endif // _USE_IMGUI




void Emitter::ImGui() {
#ifdef _USE_IMGUI

	

	std::ostringstream oss;
	oss << "Particle###" << static_cast<const void*>(this);
	const std::string windowTitle = oss.str();

	ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(windowTitle.c_str())) {
		ImGui::PushID(this);

		if (ImGui::CollapsingHeader("Emitter")) {
			if (ImGui::CollapsingHeader("Transform")) {
				ImGui::DragFloat3("Position", &emitter_.transform.translate.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::DragFloat3("Rotation", &emitter_.transform.rotate.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::DragFloat3("Scale", &emitter_.transform.scale.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
			}
			ImGui::DragInt("Count", reinterpret_cast<int*>(&emitter_.count), 1, 0, 10000);
			ImGui::DragFloat("Frequency", &emitter_.frequency, 0.01f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat("Frequency Time", &emitter_.frequencyTime, 0.01f, 0.0f, 9999.0f, "%.2f");
		}

		if (ImGui::Button("Add Particle")) {
			Emit();
		}
		ImGui::Checkbox("stop", &isStop_);
		ImGui::Checkbox("IsHit", &isHit_);
		ImGui::Separator();
		ImGui::Text("Active Particles: %d", effectDefinition_.get()->GetEffectDefinitionNum());
		ImGui::PopID();
	}
	ImGui::End();
#endif // _USE_IMGUI

}

void Emitter::Initialize() {


	randomEngine.seed(seedGenerator_());

	effectDefinition_.get()->Initialize();

}

void Emitter::Initialize(EmitterData emitter)
{
	emitter_ = emitter;

	randomEngine.seed(seedGenerator_());

	effectDefinition_.get()->Initialize();
}

void Emitter::Initialize(EmitterData emitter, EffectDefinitionData effectDefinitionData)
{
	emitter_ = emitter;

	SetEffectDefinitionData_ = effectDefinitionData;
	randomEngine.seed(seedGenerator_());

	effectDefinition_.get()->Initialize();
}

void Emitter::Initialize(EmitterData emitter, EffectDefinitionData particleData, int TextureHandle)
{
	emitter_ = emitter;
	SetEffectDefinitionData_ = particleData;
	randomEngine.seed(seedGenerator_());
	effectDefinition_.get()->Initialize();
}

void Emitter::Update(Matrix4x4 viewMatrix) {

	for (std::list<EffectDefinitionData>::iterator particleIterator = effectDefinitionData_.begin();
		particleIterator != effectDefinitionData_.end(); ) {

		// ループの先頭に追加
		if (particleIterator->currentTime >= particleIterator->lifeTime) {
			particleIterator = effectDefinitionData_.erase(particleIterator);
			continue;
		}

		// 位置を更新
		particleIterator->transform.translate += particleIterator->velocity;

		particleIterator->color.w -= 1.0f / (particleIterator->lifeTime * 60.0f);
		particleIterator->currentTime += 1.0f / 60.0f;

		++particleIterator;//これを忘れた未来の僕がいるならこれを忘れた今の僕が悲しむ
	}

	if (!isStop_) {
		emitter_.frequencyTime += 1.0f / 60.0f;
		if (emitter_.frequency <= emitter_.frequencyTime) {
			Emit();
			emitter_.frequencyTime -= emitter_.frequency;
		}
	}

	effectDefinition_.get()->Updata(viewMatrix, effectDefinitionData_);
}

void Emitter::Update(Matrix4x4 viewMatrix, std::function<EffectDefinitionData(const EffectDefinitionData&)> moveBehavior)
{
	int i = 0;
	for (std::list<EffectDefinitionData>::iterator particleIterator = effectDefinitionData_.begin();
		particleIterator != effectDefinitionData_.end(); ) {

		// ループの先頭に追加
		if (particleIterator->currentTime >= particleIterator->lifeTime) {
			particleIterator = effectDefinitionData_.erase(particleIterator);
			continue;
		}

		// 外部のムーブ関数で更新結果を受け取る
		EffectDefinitionData updated = *particleIterator;
		if (moveBehavior) {
			updated = moveBehavior(*particleIterator);
		}
		// 必要なフィールドを反映
		particleIterator->velocity = updated.velocity;


		if (OnCollision(*particleIterator)&& isHit_) {
			(*particleIterator).velocity += accelerationFiled_.acceleration / 60.0f;
		}
		


		particleIterator->transform.translate += particleIterator->velocity;
		particleIterator->transform.scale = updated.transform.scale;
		particleIterator->transform.rotate = updated.transform.rotate;

		// 共通の寿命更新
		particleIterator->color.w -= 1.0f / (particleIterator->lifeTime * 60.0f);
		particleIterator->currentTime += 1.0f / 60.0f;

		++particleIterator;
		++i;
	}
	if (!isStop_) {
		emitter_.frequencyTime += 1.0f / 60.0f;
		if (emitter_.frequency <= emitter_.frequencyTime) {
			Emit();
			emitter_.frequencyTime -= emitter_.frequency;
		}
	}

	


	effectDefinition_.get()->Updata(viewMatrix, effectDefinitionData_);
}

void Emitter::Update(EmitterData emitter, Matrix4x4 viewMatrix, std::function<EffectDefinitionData(const EffectDefinitionData&)> moveBehavior)
{
	emitter_ = emitter;

	int i = 0;
	for (std::list<EffectDefinitionData>::iterator particleIterator = effectDefinitionData_.begin();
		particleIterator != effectDefinitionData_.end(); ) {

		// ループの先頭に追加
		if (particleIterator->currentTime >= particleIterator->lifeTime) {
			particleIterator = effectDefinitionData_.erase(particleIterator);
			continue;
		}

		// 外部のムーブ関数で更新結果を受け取る
		EffectDefinitionData updated = *particleIterator;
		if (moveBehavior) {
			updated = moveBehavior(*particleIterator);
		}
		// 必要なフィールドを反映
		particleIterator->velocity = updated.velocity;
		particleIterator->transform.translate += particleIterator->velocity;
		particleIterator->transform.scale = updated.transform.scale;
		particleIterator->transform.rotate = updated.transform.rotate;

		// 共通の寿命更新
		particleIterator->color.w -= 1.0f / (particleIterator->lifeTime * 60.0f);
		particleIterator->currentTime += 1.0f / 60.0f;

		++particleIterator;
		++i;
	}
	if (!isStop_) {
		emitter_.frequencyTime += 1.0f / 60.0f;
		if (emitter_.frequency <= emitter_.frequencyTime) {
			Emit();
			emitter_.frequencyTime -= emitter_.frequency;
		}
	}
	
	effectDefinition_.get()->Updata(viewMatrix, effectDefinitionData_);
}

void Emitter::Update(Matrix4x4 viewMatrix, Vector3 scale)
{
	for (std::list<EffectDefinitionData>::iterator particleIterator = effectDefinitionData_.begin();
		particleIterator != effectDefinitionData_.end(); ) {

		// ループの先頭に追加
		if (particleIterator->currentTime >= particleIterator->lifeTime) {
			particleIterator = effectDefinitionData_.erase(particleIterator);
			continue;
		}
		else {
			++particleIterator;//これを忘れた未来の僕がいるならこれを忘れた今の僕が悲しむ
		}
		// 位置を更新
		particleIterator->transform.translate += particleIterator->velocity;

		particleIterator->color.w -= 1.0f / (particleIterator->lifeTime * 60.0f);
		particleIterator->currentTime += 1.0f / 60.0f;

		
	}
	if (!isStop_) {
		emitter_.frequencyTime += 1.0f / 60.0f;
		if (emitter_.frequency <= emitter_.frequencyTime) {
			Emit();
			emitter_.frequencyTime -= emitter_.frequency;
		}
	}

	effectDefinition_.get()->Updata(viewMatrix, effectDefinitionData_);
}

void Emitter::Draw() {
	Draw::DrawParticle(effectDefinition_.get());
}

EffectDefinitionData Emitter::MakeNewParticle()
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	EffectDefinitionData data;
	data = SetEffectDefinitionData_;
	Vector3 possion = { distribution(randomEngine),distribution(randomEngine) ,distribution(randomEngine) };
	data.transform.translate = possion * emitter_.transform.scale + emitter_.transform.translate;
	data.velocity = { distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f };

	if (generatorBehavior) {
		generatorBehavior(data);
	}

	return data;
}

EffectDefinitionData Emitter::MakeNewParticle(Vector3 scale)
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	EffectDefinitionData data;
	data.transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Vector3 possion = { distribution(randomEngine),distribution(randomEngine) ,distribution(randomEngine) };
	data.transform.translate = possion * emitter_.transform.scale + emitter_.transform.translate;
	data.transform.scale = scale;
	data.velocity = { distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f };
	data.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 色を初期化
	data.lifeTime = 3.0f; // ライフタイムを設定
	data.currentTime = 0.0f;

	if (generatorBehavior) {
		generatorBehavior(data);
	}

	return data;
}

EffectDefinitionData Emitter::particleMove(EffectDefinitionData p)
{
	p.velocity = { 0.0f,0.0f,0.0f };
	p.velocity.y -= 0.01f;
	return p;
}

EffectDefinitionData Emitter::particleMoveFire(EffectDefinitionData p)
{
	// 上昇力（炎が上に伸びる）
	p.velocity.y += 0.01f / 60.0f;

	// ランダムに左右へゆらぐ
	float randX = ((float)rand() / RAND_MAX - 0.5f) * 0.02f / 60.0f;
	float randZ = ((float)rand() / RAND_MAX - 0.5f) * 0.02f / 60.0f;
	p.velocity.x += randX;
	p.velocity.z += randZ;

	// 徐々に縮む
	p.transform.scale.x *= 0.999f;
	p.transform.scale.y *= 0.999f;
	p.transform.scale.z *= 0.999f;
	return p;

}

void Emitter::EmitSize()
{
	if (Input::PressKey(DIK_A)) {
		emitter_.transform.scale.x -= 0.01f;
	}
	if (Input::PressKey(DIK_D)) {
		emitter_.transform.scale.x += 0.01f;
	}
	if (emitter_.transform.scale.x < 0) {
		emitter_.transform.scale.x = 0;
	}
}

bool Emitter::OnCollision(EffectDefinitionData particleData)
{
	AABB aabb1 = accelerationFiled_.area;
	AABB aabb2 = GetAABB(particleData.transform, particleData.transform.scale.x, particleData.transform.scale.z);
	if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x &&
		aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y &&
		aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z) {
		return true;
	}
	return false;

}

void Emitter::Emit()
{
	for (uint32_t count = 0; count < emitter_.count; ++count) {
		effectDefinitionData_.push_back(MakeNewParticle());
	}
}

