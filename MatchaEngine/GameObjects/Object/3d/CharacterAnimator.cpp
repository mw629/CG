#include "CharacterAnimator.h"
#include "Calculation.h"
#include <Load.h>



CharacterAnimator::~CharacterAnimator()
{
}

CharacterAnimator::CharacterAnimator()
{
}

void CharacterAnimator::Initialize(const std::string& directoryPath, const std::string& filename)
{
	modelData_ = AssimpLoadObjFile(directoryPath, filename);
	animation_=LoadAnimationFile(directoryPath, filename);
	textureSrvHandleGPU_ = texture->TextureData(modelData_.textureIndex);

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMatrial();
	CreateObject();
}

void CharacterAnimator::CreateVertexData()
{
	//頂点リソースを作る
	vertexResource_ = GraphicsDevice::CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());
	//頂点バッファービューを作成する
	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * modelData_.vertices.size());
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	vertexSize_ = modelData_.vertices.size();
}

void CharacterAnimator::SettingWvp(Matrix4x4 viewMatrix) {
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 10000.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatrix));
	Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldViewProjectionMatrix));


	wvpData_->WVP = localMatrix_ * worldMatrix * worldViewProjectionMatrix;
	wvpData_->World = localMatrix_ * worldMatrix;
	wvpData_->WorldInverseTranspose = worldInverseTranspose;
}




void CharacterAnimator::Update(Matrix4x4 viewMatrix)
{
	animationTime_ += 1.0f / 60.0f;//時間を進める
	animationTime_ = std::fmod(animationTime_, animation_.duration);//リピート再生
	NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData_.rootNode.name];
	Vector3 translate = CalculateValue(rootNodeAnimation.translate, animationTime_);
	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate, animationTime_);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale, animationTime_);
	localMatrix_ = MakeAffineMatrix(translate, scale, rotate);

	SettingWvp(viewMatrix);

}


Vector3 CharacterAnimator::CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time)
{
	assert(!keyframe.empty());
	if (keyframe.size() == 1 || time <= keyframe[0].time) {//キーは一つか、時刻がキーフレーム前なら最初の値にする
		return keyframe[0].value;
	}

	for (size_t index = 0; index < keyframe.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		//IndexとnextIndexの二つがkeyframeを取得して範囲内に時刻があるかの判定
		if (keyframe[index].time <= time && time <= keyframe[nextIndex].time) {
			//範囲内を保管する
			float t = (time - keyframe[index].time) / (keyframe[nextIndex].time - keyframe[index].time);
			return Lerp(keyframe[index].value, keyframe[nextIndex].value, t);
		}
	}
	// 範囲外の場合は最後の値を返す
	return keyframe.back().value;
}

Quaternion CharacterAnimator::CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time)
{
	assert(!keyframe.empty());
	if (keyframe.size() == 1 || time <= keyframe[0].time) {//キーは一つか、時刻がキーフレーム前なら最初の値にする
		return keyframe[0].value;
	}

	for (size_t index = 0; index < keyframe.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		//IndexとnextIndexの二つがkeyframeを取得して範囲内に時刻があるかの判定
		if (keyframe[index].time <= time && time <= keyframe[nextIndex].time) {
			//範囲内を保管する
			float t = (time - keyframe[index].time) / (keyframe[nextIndex].time - keyframe[index].time);
			return Lerp(keyframe[index].value, keyframe[nextIndex].value, t);
		}
	}
	// 範囲外の場合は最後の値を返す
	return keyframe.back().value;
}

