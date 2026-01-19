#include "CharacterAnimator.h"
#include "Calculation.h"

void CharacterAnimator::Initialize(ModelData modelData, Animation animation)
{

}


void CharacterAnimator::Update()
{
	animationTime_ += 1.0f / 60.0f;//時間を進める
	animationTime_ = std::fmod(animationTime_, animation_.duration);//リピート再生
	NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData_.rootNode.name];
	Vector3 translate = CalculateValue(rootNodeAnimation.translate, animationTime_);
	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate, animationTime_);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale, animationTime_);
	Matrix4x4 localMatrix = MakeAffineMatrix(translate, scale, rotate);



}

Vector3 CharacterAnimator::CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time)
{
	return Vector3();
}
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time)
{
	return Quaternion();
}

