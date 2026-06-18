#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <unordered_map>

struct PaddedFloat {
	float v;
	float p[3];
};

struct PaddedFloat2 {
	float v[2];
	float p[2];
};

struct PostEffectShaderData {
	float time;
	float ratio;
	float value1;
	float value2;
	float blurStrength;
	int kernelSize;
	float padding[2];
	PaddedFloat kernel3x3[3][3];
	PaddedFloat kernel5x5[5][5];
	PaddedFloat2 index3x3[3][3];
	PaddedFloat2 index5x5[5][5];
};

class PostEffect {
public:
	enum class Type {
		Normal,
		GrayScale,
		Sepia,
		OutLine,
		Smoothing,
		Vignetting,
		RadialBlur,
		Dissolve,
		GaussianFilter,
		Random
	};

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_;
	PostEffectShaderData* cbData_ = nullptr;

	float time_ = 0.0f;
	float ratio_ = 1.0f;
	float value1_ = 0.0f;
	float value2_ = 0.0f;
	float blurStrength_ = 1.0f;

	float kernel3x3_[3][3];
	float kernel5x5_[5][5];
	
	float index3x3_[3][3][2];
	float index5x5_[5][5][2];
	
	int kernelSize_ = 5;
	
	bool isTimerRunning_ = true;

	std::unordered_map<std::string, std::string> texturePaths_;
	std::string maskTexturePath_ = "Resources/Texture/noise0.png";

	Type currentType_ = Type::Normal;
	static Type activeType_;
	static std::string activeShaderName_;

public:
	PostEffect();
	~PostEffect();

	void Initialize();
	void Update(float deltaTime);

	void StartTimer() { isTimerRunning_ = true; }
	void StopTimer() { isTimerRunning_ = false; }

	// Getters and Setters
	void SetTime(float time) { 
		time_ = time;
		if (cbData_) cbData_->time = time_;
	}
	float GetTime() const { return time_; }

	void SetRatio(float ratio) { 
		ratio_ = ratio;
		if (cbData_) cbData_->ratio = ratio_;
	}
	float GetRatio() const { return ratio_; }

	void SetValue1(float val) { 
		value1_ = val;
		if (cbData_) cbData_->value1 = value1_;
	}
	float GetValue1() const { return value1_; }

	void SetValue2(float val) { 
		value2_ = val;
		if (cbData_) cbData_->value2 = value2_;
	}
	float GetValue2() const { return value2_; }

	void SetTexturePath(const std::string& name, const std::string& filePath);

	std::string GetTexturePath(const std::string& name) const {
		auto it = texturePaths_.find(name);
		if (it != texturePaths_.end()) {
			return it->second;
		}
		return "";
	}

	void SetMaskTexturePath(const std::string& filePath);
	std::string GetMaskTexturePath() const { return maskTexturePath_; }

	const std::unordered_map<std::string, std::string>& GetTexturePaths() const {
		return texturePaths_;
	}

	ID3D12Resource* GetConstantBufferResource() const { return constantBufferResource_.Get(); }

	void ImGuiWindow();

	// Static methods for managing active post effect
	static void SetActivePostEffect(Type type);
	static void SetActivePostEffectByName(const std::string& name) { activeShaderName_ = name; }
	static Type GetActivePostEffect() { return activeType_; }
	static std::string GetActiveShaderName() { return activeShaderName_; }

	void SetBlurStrength(float strength) {
		blurStrength_ = strength;
		if (cbData_) cbData_->blurStrength = blurStrength_;
	}
	float GetBlurStrength() const { return blurStrength_; }

	// Convenience method to set all parameters at once
	void SetPostEffectParameters(float time, float ratio, float value1, float value2, float blurStrength = 1.0f) {
		time_ = time;
		ratio_ = ratio;
		value1_ = value1;
		value2_ = value2;
		blurStrength_ = blurStrength;
		if (cbData_) {
			cbData_->time = time_;
			cbData_->ratio = ratio_;
			cbData_->value1 = value1_;
			cbData_->value2 = value2_;
			cbData_->blurStrength = blurStrength_;
			cbData_->kernelSize = kernelSize_;
		}
	}
};