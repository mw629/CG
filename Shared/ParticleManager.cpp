#include "ParticleManager.h"
#include <functional>

#ifdef _USE_IMGUI
#include <imgui.h>
#include <sstream>
#endif // _USE_IMGUI




void ParticleManager::ImGui() {
#ifdef _USE_IMGUI
	std::ostringstream oss;
	oss << "Particle###" << static_cast<const void*>(this);
	const std::string windowTitle = oss.str();

	if (ImGui::Begin(windowTitle.c_str())) {
		ImGui::PushID(this);

		if (ImGui::CollapsingHeader("Emitter")) {
			if (ImGui::CollapsingHeader("Transform")) {
				ImGui::DragFloat3("Position", &emitter_.transform.translate.x, 0.01f);
				ImGui::DragFloat3("Rotation", &emitter_.transform.rotate.x, 0.01f);
				ImGui::DragFloat3("Scale", &emitter_.transform.scale.x, 0.01f);
			}
			ImGui::DragInt("Count", reinterpret_cast<int*>(&emitter_.count), 1, 0, 10000);
			ImGui::DragFloat("Frequency", &emitter_.frequency, 0.01f, 0.0f, 10.0f);
			ImGui::DragFloat("Frequency Time", &emitter_.frequencyTime, 0.01f, 0.0f, 9999.0f);
		}

		if (ImGui::Button("Add Particle")) {
			Emit();
		}
		ImGui::Checkbox("stop", &isStop_);
		ImGui::Separator();
		ImGui::Text("Active Particles: %d", particle_.get()->GetParticleNum());
		ImGui::PopID();
	}
	ImGui::End();
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

void ParticleManager::Initialize(Emitter emitter, ParticleData particleData)
{
	emitter_ = emitter;

	SetParticleData_ = particleData;

	randomEngine.seed(seedGenerator_());

	particle_.get()->Initialize();
}

void ParticleManager::Initialize(Emitter emitter, ParticleData particleData, int TextureHandle)
{
	emitter_ = emitter;
	SetParticleData_ = particleData;
	randomEngine.seed(seedGenerator_());
	particle_.get()->Initialize(TextureHandle);
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

	if (!isStop_) {
		emitter_.frequencyTime += 1.0f / 60.0f;
		if (emitter_.frequency <= emitter_.frequencyTime) {
			Emit();
			emitter_.frequencyTime -= emitter_.frequency;
		}
	}

	particle_.get()->Updata(viewMatrix, particleData_);
}

void ParticleManager::Update(Matrix4x4 viewMatrix, int a)
{
	int i = 0;
	for (std::list<ParticleData>::iterator particleIterator = particleData_.begin();
		particleIterator != particleData_.end(); ) {

		// 外部のムーブ関数で更新結果を受け取る
		ParticleData updated;
		if (a == 0) {
			updated = particleMoveFire(*particleIterator);
		}
		if (a == 1) {
			updated = particleMove(*particleIterator);
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
	particle_.get()->Updata(viewMatrix, particleData_);
}

void ParticleManager::Update(Emitter emitter, Matrix4x4 viewMatrix, int a)
{
	emitter_ = emitter;

	int i = 0;
	for (std::list<ParticleData>::iterator particleIterator = particleData_.begin();
		particleIterator != particleData_.end(); ) {

		// 外部のムーブ関数で更新結果を受け取る
		ParticleData updated;
		if (a == 0) {
			updated = particleMoveFire(*particleIterator);
		}
		if (a == 1) {
			updated = particleMove(*particleIterator);
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
	if (!isStop_) {
		emitter_.frequencyTime += 1.0f / 60.0f;
		if (emitter_.frequency <= emitter_.frequencyTime) {
			Emit();
			emitter_.frequencyTime -= emitter_.frequency;
		}
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
	data = SetParticleData_;
	Vector3 possion = { distribution(randomEngine),distribution(randomEngine) ,distribution(randomEngine) };
	data.transform.translate = possion * emitter_.transform.scale + emitter_.transform.translate;
	data.velocity = { distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f ,distribution(randomEngine) / 60.0f };
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

ParticleData ParticleManager::particleMove(ParticleData p)
{
	p.velocity = { 0.0f,0.0f,0.0f };
	p.velocity.y -= 0.01f;
	return p;
}

ParticleData ParticleManager::particleMoveFire(ParticleData p)
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

void ParticleManager::EmitSize()
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

void ParticleManager::OnCollision(ParticleManager* particle)
{
	for (auto& a : particleData_) {

		Vector3 posA = a.transform.translate;
		float radiusA = a.transform.scale.x; // 半径
		int index = 0;
		for (auto& b : particle->GetParticleData()) {

			Vector3 posB = b.transform.translate;
			float radiusB = b.transform.scale.x; // 半径

			// A → B へのベクトル
			Vector3 d = {
				posB.x - posA.x,
				posB.y - posA.y,
				posB.z - posA.z
			};

			// 距離の2乗
			float distSq = d.x * d.x + d.y * d.y + d.z * d.z;

			// 半径の合計
			float r = radiusA + radiusB;

			// 衝突チェック
			if (distSq <= r * r) {
				particle_.get()->DeleteParticle(index);
			}
		}
		index += 1;
	}

}

void ParticleManager::Emit()
{
	for (uint32_t count = 0; count < emitter_.count; ++count) {
		particleData_.push_back(MakeNewParticle());
	}
}

