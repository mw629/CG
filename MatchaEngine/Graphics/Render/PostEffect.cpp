#include "PostEffect.h"
#include "../GraphicsDevice.h"
#include "../../Resource/Texture.h"
#include <imgui.h>


std::vector<PostEffect*> PostEffect::s_instances;

PostEffect::PostEffect() {
	s_instances.push_back(this);
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			kernel3x3_[y][x] = 1.0f / 9.0f;
		}
	}
	for (int y = 0; y < 5; ++y) {
		for (int x = 0; x < 5; ++x) {
			kernel5x5_[y][x] = 1.0f / 25.0f;
		}
	}
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			index3x3_[y][x][0] = (float)(x - 1);
			index3x3_[y][x][1] = (float)(y - 1);
		}
	}
	for (int y = 0; y < 5; ++y) {
		for (int x = 0; x < 5; ++x) {
			index5x5_[y][x][0] = (float)(x - 2);
			index5x5_[y][x][1] = (float)(y - 2);
		}
	}
}

PostEffect::~PostEffect() {
	auto it = std::find(s_instances.begin(), s_instances.end(), this);
	if (it != s_instances.end()) {
		s_instances.erase(it);
	}
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
		cbData_->blurStrength = blurStrength_;
		cbData_->kernelSize = kernelSize_;
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				cbData_->kernel3x3[y][x].v = kernel3x3_[y][x];
			}
		}
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				cbData_->kernel5x5[y][x].v = kernel5x5_[y][x];
			}
		}
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				cbData_->index3x3[y][x].v[0] = index3x3_[y][x][0];
				cbData_->index3x3[y][x].v[1] = index3x3_[y][x][1];
			}
		}
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				cbData_->index5x5[y][x].v[0] = index5x5_[y][x][0];
				cbData_->index5x5[y][x].v[1] = index5x5_[y][x][1];
			}
		}
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
		cbData_->blurStrength = blurStrength_;
		cbData_->kernelSize = kernelSize_;
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				cbData_->kernel3x3[y][x].v = kernel3x3_[y][x];
			}
		}
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				cbData_->kernel5x5[y][x].v = kernel5x5_[y][x];
			}
		}
		for (int y = 0; y < 3; ++y) {
			for (int x = 0; x < 3; ++x) {
				cbData_->index3x3[y][x].v[0] = index3x3_[y][x][0];
				cbData_->index3x3[y][x].v[1] = index3x3_[y][x][1];
			}
		}
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				cbData_->index5x5[y][x].v[0] = index5x5_[y][x][0];
				cbData_->index5x5[y][x].v[1] = index5x5_[y][x][1];
			}
		}
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
#ifdef _USE_IMGUI
	if (ImGui::TreeNode(("Post Effect Parameters##" + std::to_string((size_t)this)).c_str())) {
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
			cbData_->blurStrength = blurStrength_;
			cbData_->kernelSize = kernelSize_;
		}

		if (activeType_ == Type::Smoothing || activeType_ == Type::GaussianFilter) {
			ImGui::SliderFloat("Blur Strength##PostEffect", &blurStrength_, 0.0f, 10.0f);

			ImGui::Text("Kernel Size:"); ImGui::SameLine();
			ImGui::RadioButton("3x3##KernelSize", &kernelSize_, 3); ImGui::SameLine();
			ImGui::RadioButton("5x5##KernelSize", &kernelSize_, 5);
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
	if (ImGui::TreeNode(("Post Effect Selection##" + std::to_string((size_t)this)).c_str())) {
		int currentEffectIndex = static_cast<int>(activeType_);
		const char* effectNames[] = { "Normal", "GrayScale", "Sepia", "OutLine", "Smoothing", "Vignetting", "RadialBlur", "Dissolve", "GaussianFilter","Random"};

		if (ImGui::Combo("Post Effect Type", &currentEffectIndex, effectNames, IM_ARRAYSIZE(effectNames))) {
			activeType_ = static_cast<Type>(currentEffectIndex);
			switch (activeType_) {
			case Type::Normal:         activeShaderName_ = "CopyShader"; break;
			case Type::GrayScale:      activeShaderName_ = "GrayScaleShader"; break;
			case Type::Sepia:          activeShaderName_ = "GrayScaleSepiaToneShader"; break;
			case Type::OutLine:        activeShaderName_ = "OutLineShader"; break;
			case Type::Smoothing:      activeShaderName_ = "SmoothingShader"; break;
			case Type::Vignetting:     activeShaderName_ = "VignettingShader"; break;
			case Type::RadialBlur:     activeShaderName_ = "RadialBlurShader"; break;
			case Type::Dissolve:       activeShaderName_ = "DissolveShader"; break;
			case Type::GaussianFilter: activeShaderName_ = "GaussianFilterShader"; break;
			case Type::Random:         activeShaderName_ = "RandomShader"; break;
			default:                   activeShaderName_ = "CopyShader"; break;
			}
		}

		ImGui::TreePop();
	}
#endif
}

void PostEffect::SetActivePostEffect(Type type) {
	if (!s_instances.empty()) {
		s_instances[0]->activeType_ = type;
		switch (type) {
		case Type::Normal:         s_instances[0]->activeShaderName_ = "CopyShader"; break;
		case Type::GrayScale:      s_instances[0]->activeShaderName_ = "GrayScaleShader"; break;
		case Type::Sepia:          s_instances[0]->activeShaderName_ = "GrayScaleSepiaToneShader"; break;
		case Type::OutLine:        s_instances[0]->activeShaderName_ = "OutLineShader"; break;
		case Type::Smoothing:      s_instances[0]->activeShaderName_ = "SmoothingShader"; break;
		case Type::Vignetting:     s_instances[0]->activeShaderName_ = "VignettingShader"; break;
		case Type::RadialBlur:     s_instances[0]->activeShaderName_ = "RadialBlurShader"; break;
		case Type::Dissolve:       s_instances[0]->activeShaderName_ = "DissolveShader"; break;
		case Type::GaussianFilter: s_instances[0]->activeShaderName_ = "GaussianFilterShader"; break;
		case Type::Random:         s_instances[0]->activeShaderName_ = "RandomShader"; break;
		default:                   s_instances[0]->activeShaderName_ = "CopyShader"; break;
		}
	}
}
