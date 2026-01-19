#pragma once
#include <VariableTypes.h>
#include <Model.h>

class CharacterAnimator
{
private:

	ModelData modelData_;
	Animation animation_;

	float animationTime_ = 0.0f;

public:

	void Initialize(ModelData modelData, Animation animation);

	void Update();


	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

};

