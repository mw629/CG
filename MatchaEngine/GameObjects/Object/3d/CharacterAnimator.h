#pragma once
#include <VariableTypes.h>
#include "ObjectBase.h"
#include <Model.h>

class CharacterAnimator:public ObjectBase
{
private:

	ModelData modelData_{};

	Animation animation_;
	float animationTime_ = 0.0f;

	Matrix4x4 localMatrix_;

public:

	~CharacterAnimator()override;
	CharacterAnimator();

	void Initialize(const std::string& directoryPath, const std::string& filename);

	void CreateVertexData()override;
	void SettingWvp(Matrix4x4 viewMatrix) override;

	void Update(Matrix4x4 viewMatrix);



	ModelData GetModelData() { return modelData_; }

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

};

