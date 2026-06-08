#include "PostEffect.h"
#include "../GraphicsDevice.h"
#include "../../Resource/Texture.h"
#include <imgui.h>

// Static member initialization
PostEffect::Type PostEffect::activeType_ = PostEffect::Type::Normal;
std::string PostEffect::activeShaderName_ = "CopyShader";

PostEffect::PostEffect() {
}

PostEffect::~PostEffect() {
	if (constantBufferResource_) {
		constantBufferResource_->Unmap(0, nullptr);
	}
}

void PostEffect::Initialize() {
	constantBufferResource_ = GraphicsDevice::CreateBufferResource(sizeof(PostEffectShaderData));
	cbData_ = nullptr;
	constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&cbData_));

	if (cbData_) {
		cbData_->time = time_;
		cbData_->ratio = ratio_;
		cbData_->value1 = value1_;
		cbData_->value2 = value2_;
	}

	SetMaskTexturePath(maskTexturePath_);
}

void PostEffect::Update(float deltaTime) {
	if (isTimerRunning_) {
		time_ += deltaTime;
	}

	if (cbData_) {
		cbData_->time = time_;
		cbData_->ratio = ratio_;
		cbData_->value1 = value1_;
		cbData_->value2 = value2_;
	}
}

void PostEffect::SetTexturePath(const std::string& name, const std::string& filePath) {
	texturePaths_[name] = filePath;

	// Create/Load texture immediately if it is not already loaded
	Texture texture;
	texture.CreateTexture(filePath);
}

void PostEffect::SetMaskTexturePath(const std::string& filePath) {
	maskTexturePath_ = filePath;
	SetTexturePath("gMaskTexture", filePath);
}

void PostEffect::ImGuiWindow() {
	if (ImGui::TreeNode("Post Effect Parameters")) {
		ImGui::SliderFloat("Time##PostEffect", &time_, 0.0f, 100.0f);
		
		ImGui::Checkbox("Timer Running", &isTimerRunning_);
		if (ImGui::Button("Start Timer")) { StartTimer(); }
		ImGui::SameLine();
		if (ImGui::Button("Stop Timer")) { StopTimer(); }

		ImGui::SliderFloat("Ratio##PostEffect", &ratio_, 0.0f, 1.0f);
		ImGui::SliderFloat("Value1##PostEffect", &value1_, 0.0f, 10.0f);
		ImGui::SliderFloat("Value2##PostEffect", &value2_, 0.0f, 10.0f);

		if (cbData_) {
			cbData_->time = time_;
			cbData_->ratio = ratio_;
			cbData_->value1 = value1_;
			cbData_->value2 = value2_;
		}

		if (activeType_ == Type::Dissolve) {
			char buffer[256];
			strcpy_s(buffer, sizeof(buffer), maskTexturePath_.c_str());
			if (ImGui::InputText("Mask Texture##PostEffect", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
				SetMaskTexturePath(buffer);
			}
		}

		ImGui::TreePop();
	}

	// Post Effect type selection
	if (ImGui::TreeNode("Post Effect Selection")) {
		static int currentEffectIndex = static_cast<int>(activeType_);
		const char* effectNames[] = { "Normal", "GrayScale", "Sepia", "OutLine", "Smoothing", "Vignetting", "RadialBlur", "Dissolve", "GaussianFilter","Random"};

		if (ImGui::Combo("Post Effect", &currentEffectIndex, effectNames, IM_ARRAYSIZE(effectNames))) {
			SetActivePostEffect(static_cast<Type>(currentEffectIndex));
		}

		ImGui::TreePop();
	}
}

void PostEffect::SetActivePostEffect(Type type) {
	activeType_ = type;
	switch (type) {
	case Type::Normal: activeShaderName_ = "CopyShader"; break;
	case Type::GrayScale: activeShaderName_ = "GrayScaleShader"; break;
	case Type::Sepia: activeShaderName_ = "GrayScaleSepiaToneShader"; break;
	case Type::OutLine: activeShaderName_ = "OutLineShader"; break;
	case Type::Smoothing: activeShaderName_ = "SmoothingShader"; break;
	case Type::Vignetting: activeShaderName_ = "VignettingShader"; break;
	case Type::RadialBlur: activeShaderName_ = "RadialBlurShader"; break;
	case Type::Dissolve: activeShaderName_ = "DissolveShader"; break;
	case Type::GaussianFilter: activeShaderName_ = "GaussianFilterShader"; break;
	case Type::Random: activeShaderName_ = "RandomShader"; break;
	}
}

