#pragma once
#include"Object3DBase.h"


class Model	:public Object3DBase
{
private:
	ModelData modelData_{};
	

public:
	
	~Model()override;

	void ImGui();

	static void SetDevice(ID3D12Device* device);
	static void SetScreenSize(Vector2 screenSize);

	void Initialize(ModelData modelData);

	void CreateVertexData()override;
	void SettingWvp(Matrix4x4 viewMatrix) override;


	ModelData GetModelData() { return modelData_; }
	

private:
	ShaderName shader_ = ShaderName::ObjectShader;
	BlendMode blend_ = BlendMode::kBlendModeNone;
public:
	void SetBlend(BlendMode blend) { blend_ = blend; }
	ShaderName GetShader() { return shader_; }
	BlendMode GetBlend() { return blend_; }
	
};

