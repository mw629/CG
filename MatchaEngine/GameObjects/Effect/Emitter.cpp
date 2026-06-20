#include "Emitter.h"
#include <functional>
#include "Input.h"
#include "Calculation.h"
#include "Draw.h"

#ifdef _USE_IMGUI
#include <imgui.h>
#include <sstream>
#endif // _USE_IMGUI

#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>


void Emitter::SetTexturePath(const std::string& path)
{
	texturePath_ = path;
	effectDefinition_->SetTexturePath(path);
}

void Emitter::SetShape(EffectShape shape)
{
	shape_ = shape;
	effectDefinition_->SetShape(shape);
}

void Emitter::ImGui() {
#ifdef _USE_IMGUI

	ImGui::PushID(this);
	if (ImGui::CollapsingHeader(name_.c_str())) {
		if (ImGui::TreeNode("Emitter Settings")) {
			if (ImGui::TreeNode("Transform")) {
				ImGui::DragFloat3("Position", &emitter_.transform.translate.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::DragFloat3("Rotation", &emitter_.transform.rotate.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::DragFloat3("Scale", &emitter_.transform.scale.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
				ImGui::TreePop();
			}
			ImGui::DragInt("Count", reinterpret_cast<int*>(&emitter_.count), 1, 0, 10000);
			ImGui::DragFloat("Frequency", &emitter_.frequency, 0.01f, 0.0f, 10.0f, "%.2f");
			ImGui::DragFloat("Frequency Time", &emitter_.frequencyTime, 0.01f, 0.0f, 9999.0f, "%.2f");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Particle Initial Settings")) {
			ImGui::DragFloat3("Scale", &SetEffectDefinitionData_.transform.scale.x, 0.01f, -FLT_MAX, FLT_MAX, "%.2f");
			ImGui::ColorEdit4("Color", &SetEffectDefinitionData_.color.x);
			ImGui::DragFloat("LifeTime", &SetEffectDefinitionData_.lifeTime, 0.01f, 0.0f, FLT_MAX, "%.2f");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Particle Movement Settings")) {
			ImGui::DragFloat3("Base Velocity", &movementData_.baseVelocity.x, 0.001f, -FLT_MAX, FLT_MAX, "%.3f");
			ImGui::DragFloat3("Velocity Variance", &movementData_.velocityVariance.x, 0.001f, 0.0f, FLT_MAX, "%.3f");
			ImGui::DragFloat3("Acceleration (Gravity)", &movementData_.acceleration.x, 0.0001f, -FLT_MAX, FLT_MAX, "%.4f");
			ImGui::DragFloat("Size Multiplier", &movementData_.sizeDelta, 0.001f, 0.0f, FLT_MAX, "%.3f");
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Visual Settings")) {
			const char* shapes[] = { "Plane", "Cylinder", "Ring" };
			int currentShape = static_cast<int>(shape_);
			if (ImGui::Combo("Shape", &currentShape, shapes, IM_ARRAYSIZE(shapes))) {
				SetShape(static_cast<EffectShape>(currentShape));
			}
			
			ImGui::Text("Texture:");
			std::string btnLabel = texturePath_ + "##TexDrop";
			ImGui::Button(btnLabel.c_str(), ImVec2(-FLT_MIN, 0)); // Button taking full width

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_FILE")) {
					std::string path((const char*)payload->Data, payload->DataSize - 1);
					if (path.find(".png") != std::string::npos || path.find(".jpg") != std::string::npos || path.find(".dds") != std::string::npos) {
						SetTexturePath(path);
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::TextDisabled("(Drag and drop an image from the Resources window)");
			
			ImGui::TreePop();
		}

		if (ImGui::Button("Add Particle")) {
			Emit();
		}
		ImGui::Checkbox("stop", &isStop_);
		ImGui::Checkbox("IsHit", &isHit_);
		ImGui::Separator();
		ImGui::Text("Active Particles: %d", effectDefinition_.get()->GetEffectDefinitionNum());

		ImGui::Separator();
		ImGui::InputText("File Name", saveFileName_, sizeof(saveFileName_));

		std::filesystem::path dir = "Resources/Json/Particle";
		std::vector<std::string> jsonFiles;
		if (std::filesystem::exists(dir)) {
			for (const auto& entry : std::filesystem::directory_iterator(dir)) {
				if (entry.path().extension() == ".json") {
					jsonFiles.push_back(entry.path().filename().string());
				}
			}
		}

		if (ImGui::BeginCombo("Saved Particles", saveFileName_)) {
			for (const auto& file : jsonFiles) {
				std::string nameWithoutExt = file;
				size_t extPos = nameWithoutExt.find(".json");
				if (extPos != std::string::npos) {
					nameWithoutExt = nameWithoutExt.substr(0, extPos);
				}
				
				bool is_selected = (std::string(saveFileName_) == nameWithoutExt);
				if (ImGui::Selectable(file.c_str(), is_selected)) {
					strncpy_s(saveFileName_, sizeof(saveFileName_), nameWithoutExt.c_str(), _TRUNCATE);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Save JSON")) {
			SaveToJson(saveFileName_);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load JSON")) {
			LoadFromJson(saveFileName_);
		}
	}
	ImGui::PopID();
#endif // _USE_IMGUI

}

void Emitter::SaveToJson(const std::string& name)
{
	std::filesystem::path dir = "Resources/Json/Particle";
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}

	std::string filepath = dir.string() + "/" + name;
	if (filepath.find(".json") == std::string::npos) {
		filepath += ".json";
	}

	nlohmann::json root;

	root["emitter"]["transform"]["translate"] = { emitter_.transform.translate.x, emitter_.transform.translate.y, emitter_.transform.translate.z };
	root["emitter"]["transform"]["rotate"] = { emitter_.transform.rotate.x, emitter_.transform.rotate.y, emitter_.transform.rotate.z };
	root["emitter"]["transform"]["scale"] = { emitter_.transform.scale.x, emitter_.transform.scale.y, emitter_.transform.scale.z };
	root["emitter"]["count"] = emitter_.count;
	root["emitter"]["frequency"] = emitter_.frequency;

	root["particle"]["transform"]["scale"] = { SetEffectDefinitionData_.transform.scale.x, SetEffectDefinitionData_.transform.scale.y, SetEffectDefinitionData_.transform.scale.z };
	root["particle"]["color"] = { SetEffectDefinitionData_.color.x, SetEffectDefinitionData_.color.y, SetEffectDefinitionData_.color.z, SetEffectDefinitionData_.color.w };
	root["particle"]["lifeTime"] = SetEffectDefinitionData_.lifeTime;

	root["movement"]["baseVelocity"] = { movementData_.baseVelocity.x, movementData_.baseVelocity.y, movementData_.baseVelocity.z };
	root["movement"]["velocityVariance"] = { movementData_.velocityVariance.x, movementData_.velocityVariance.y, movementData_.velocityVariance.z };
	root["movement"]["acceleration"] = { movementData_.acceleration.x, movementData_.acceleration.y, movementData_.acceleration.z };
	root["movement"]["sizeDelta"] = movementData_.sizeDelta;

	root["visual"]["texturePath"] = texturePath_;
	root["visual"]["shape"] = static_cast<int>(shape_);

	std::ofstream file(filepath);
	if (file.is_open()) {
		file << root.dump(4);
	}
}

void Emitter::LoadFromJson(const std::string& name)
{
	std::string filepath = "Resources/Json/Particle/" + name;
	if (filepath.find(".json") == std::string::npos) {
		filepath += ".json";
	}

	std::ifstream file(filepath);
	if (!file.is_open()) return;

	nlohmann::json root;
	file >> root;

	if (root.contains("emitter")) {
		if (root["emitter"].contains("transform")) {
			emitter_.transform.translate.x = root["emitter"]["transform"]["translate"][0];
			emitter_.transform.translate.y = root["emitter"]["transform"]["translate"][1];
			emitter_.transform.translate.z = root["emitter"]["transform"]["translate"][2];

			emitter_.transform.rotate.x = root["emitter"]["transform"]["rotate"][0];
			emitter_.transform.rotate.y = root["emitter"]["transform"]["rotate"][1];
			emitter_.transform.rotate.z = root["emitter"]["transform"]["rotate"][2];

			emitter_.transform.scale.x = root["emitter"]["transform"]["scale"][0];
			emitter_.transform.scale.y = root["emitter"]["transform"]["scale"][1];
			emitter_.transform.scale.z = root["emitter"]["transform"]["scale"][2];
		}
		emitter_.count = root["emitter"]["count"];
		emitter_.frequency = root["emitter"]["frequency"];
	}

	if (root.contains("particle")) {
		if (root["particle"].contains("transform")) {
			SetEffectDefinitionData_.transform.scale.x = root["particle"]["transform"]["scale"][0];
			SetEffectDefinitionData_.transform.scale.y = root["particle"]["transform"]["scale"][1];
			SetEffectDefinitionData_.transform.scale.z = root["particle"]["transform"]["scale"][2];
		}
		if (root["particle"].contains("color")) {
			SetEffectDefinitionData_.color.x = root["particle"]["color"][0];
			SetEffectDefinitionData_.color.y = root["particle"]["color"][1];
			SetEffectDefinitionData_.color.z = root["particle"]["color"][2];
			SetEffectDefinitionData_.color.w = root["particle"]["color"][3];
		}
		SetEffectDefinitionData_.lifeTime = root["particle"]["lifeTime"];
	}

	if (root.contains("movement")) {
		if (root["movement"].contains("baseVelocity")) {
			movementData_.baseVelocity.x = root["movement"]["baseVelocity"][0];
			movementData_.baseVelocity.y = root["movement"]["baseVelocity"][1];
			movementData_.baseVelocity.z = root["movement"]["baseVelocity"][2];
		}
		if (root["movement"].contains("velocityVariance")) {
			movementData_.velocityVariance.x = root["movement"]["velocityVariance"][0];
			movementData_.velocityVariance.y = root["movement"]["velocityVariance"][1];
			movementData_.velocityVariance.z = root["movement"]["velocityVariance"][2];
		}
		if (root["movement"].contains("acceleration")) {
			movementData_.acceleration.x = root["movement"]["acceleration"][0];
			movementData_.acceleration.y = root["movement"]["acceleration"][1];
			movementData_.acceleration.z = root["movement"]["acceleration"][2];
		}
		if (root["movement"].contains("sizeDelta")) {
			movementData_.sizeDelta = root["movement"]["sizeDelta"];
		}
	} else if (root.contains("particle") && root["particle"].contains("velocity")) {
		// Fallback for old save format
		movementData_.baseVelocity.x = root["particle"]["velocity"][0];
		movementData_.baseVelocity.y = root["particle"]["velocity"][1];
		movementData_.baseVelocity.z = root["particle"]["velocity"][2];
	}

	if (root.contains("visual")) {
		if (root["visual"].contains("texturePath")) {
			SetTexturePath(root["visual"]["texturePath"].get<std::string>());
		}
		if (root["visual"].contains("shape")) {
			SetShape(static_cast<EffectShape>(root["visual"]["shape"].get<int>()));
		}
	}
}

void Emitter::Initialize(EffectShape shape) {
	shape_ = shape;
	randomEngine.seed(seedGenerator_());
	effectDefinition_.get()->Initialize(shape_);
}

void Emitter::Initialize(EmitterData emitter, EffectShape shape)
{
	shape_ = shape;
	emitter_ = emitter;
	randomEngine.seed(seedGenerator_());
	effectDefinition_.get()->Initialize(shape_);
}

void Emitter::Initialize(EmitterData emitter, EffectDefinitionData effectDefinitionData, EffectShape shape)
{
	shape_ = shape;
	emitter_ = emitter;
	SetEffectDefinitionData_ = effectDefinitionData;
	randomEngine.seed(seedGenerator_());
	effectDefinition_.get()->Initialize(shape_);
}

void Emitter::Initialize(EmitterData emitter, EffectDefinitionData particleData, int TextureHandle, EffectShape shape)
{
	emitter_ = emitter;
	SetEffectDefinitionData_ = particleData;
	randomEngine.seed(seedGenerator_());
	effectDefinition_.get()->Initialize(TextureHandle, shape);
}

void Emitter::Update(Matrix4x4 viewMatrix) {

	for (std::list<EffectDefinitionData>::iterator particleIterator = effectDefinitionData_.begin();
		particleIterator != effectDefinitionData_.end(); ) {

		// ループの先頭に追加
		if (particleIterator->currentTime >= particleIterator->lifeTime) {
			particleIterator = effectDefinitionData_.erase(particleIterator);
			continue;
		}

		// 位置とスケールを更新
		particleIterator->velocity += movementData_.acceleration;
		particleIterator->transform.scale.x *= movementData_.sizeDelta;
		particleIterator->transform.scale.y *= movementData_.sizeDelta;
		particleIterator->transform.scale.z *= movementData_.sizeDelta;
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
		particleIterator->velocity = updated.velocity + movementData_.acceleration;

		if (OnCollision(*particleIterator)&& isHit_) {
			(*particleIterator).velocity += accelerationFiled_.acceleration / 60.0f;
		}

		particleIterator->transform.translate += particleIterator->velocity;
		particleIterator->transform.scale.x = updated.transform.scale.x * movementData_.sizeDelta;
		particleIterator->transform.scale.y = updated.transform.scale.y * movementData_.sizeDelta;
		particleIterator->transform.scale.z = updated.transform.scale.z * movementData_.sizeDelta;
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
		particleIterator->velocity = updated.velocity + movementData_.acceleration;
		particleIterator->transform.translate += particleIterator->velocity;
		particleIterator->transform.scale.x = updated.transform.scale.x * movementData_.sizeDelta;
		particleIterator->transform.scale.y = updated.transform.scale.y * movementData_.sizeDelta;
		particleIterator->transform.scale.z = updated.transform.scale.z * movementData_.sizeDelta;
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
		// 位置とスケールを更新
		particleIterator->velocity += movementData_.acceleration;
		particleIterator->transform.scale.x *= movementData_.sizeDelta;
		particleIterator->transform.scale.y *= movementData_.sizeDelta;
		particleIterator->transform.scale.z *= movementData_.sizeDelta;
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
	data.velocity = { 
		movementData_.baseVelocity.x + distribution(randomEngine) * movementData_.velocityVariance.x,
		movementData_.baseVelocity.y + distribution(randomEngine) * movementData_.velocityVariance.y,
		movementData_.baseVelocity.z + distribution(randomEngine) * movementData_.velocityVariance.z 
	};

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
	data.velocity = { 
		movementData_.baseVelocity.x + distribution(randomEngine) * movementData_.velocityVariance.x,
		movementData_.baseVelocity.y + distribution(randomEngine) * movementData_.velocityVariance.y,
		movementData_.baseVelocity.z + distribution(randomEngine) * movementData_.velocityVariance.z 
	};
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

