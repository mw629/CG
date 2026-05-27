#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <unordered_map>

struct PostEffectShaderData {
	float time;
	float ratio;
	float value1;
	float value2;
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
		GaussianFilter
	};

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBufferResource_;
	PostEffectShaderData* cbData_ = nullptr;

	float time_ = 0.0f;
	float ratio_ = 1.0f;
	float value1_ = 0.0f;
	float value2_ = 0.0f;

	std::unordered_map<std::string, std::string> texturePaths_;

	Type currentType_ = Type::Normal;
	static Type activeType_;
	static std::string activeShaderName_;

public:
	PostEffect();
	~PostEffect();

	void Initialize();
	void Update(float deltaTime);

	// Getters and Setters
	void SetTime(float time) { time_ = time; }
	float GetTime() const { return time_; }

	void SetRatio(float ratio) { ratio_ = ratio; }
	float GetRatio() const { return ratio_; }

	void SetValue1(float val) { value1_ = val; }
	float GetValue1() const { return value1_; }

	void SetValue2(float val) { value2_ = val; }
	float GetValue2() const { return value2_; }

	void SetTexturePath(const std::string& name, const std::string& filePath);

	std::string GetTexturePath(const std::string& name) const {
		auto it = texturePaths_.find(name);
		if (it != texturePaths_.end()) {
			return it->second;
		}
		return "";
	}

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
};