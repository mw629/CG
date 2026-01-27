#include "CharacterAnimator.h"
#include "Calculation.h"
#include "DescriptorHeap.h"
#include <Load.h>
#include <algorithm>

namespace {
	ID3D12Device* device;
	DescriptorHeap* descriptorHeap; 
}

CharacterAnimator::~CharacterAnimator()
{
}

CharacterAnimator::CharacterAnimator()
{
}

void CharacterAnimator::SetData(ID3D12Device* SetDevice, DescriptorHeap* SetDescriptorHeap)
{
	device = SetDevice;
	descriptorHeap = SetDescriptorHeap;
}

void CharacterAnimator::Initialize(const std::string& directoryPath, const std::string& filename)
{
	modelData_ = AssimpLoadObjFile(directoryPath, filename);
	animation_ = LoadAnimationFile(directoryPath, filename);
	textureSrvHandleGPU_ = texture->TextureData(modelData_.textureIndex);

	skeleton_ = CreateSkeleton(modelData_.rootNode);
	CreateSkinCluster();

	material_ = std::make_unique<MaterialFactory>();
	material_->CreateMatrial();
	CreateObject();

	SetShader(AnimationObj);

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

void CharacterAnimator::CreateIndexResource()
{
	indexResource_ = GraphicsDevice::CreateBufferResource(sizeof(uint32_t) * modelData_.indices.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	std::memcpy(indexData_, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());


}

void CharacterAnimator::SettingWvp(Matrix4x4 viewMatrix) {
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 10000.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatrix));
	Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldViewProjectionMatrix));


	wvpData_->WVP = worldMatrix * worldViewProjectionMatrix;
	wvpData_->World = localMatrix_ * worldMatrix;
	wvpData_->WorldInverseTranspose = worldInverseTranspose;
}

Skeleton CharacterAnimator::CreateSkeleton(const Node& rootNode)
{
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}
	return skeleton;

}

int32_t CharacterAnimator::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = IdentityMatrix();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());
	joint.parent = parent;
	joints.push_back(joint);
	for (const Node& child : node.children) {
		//子Jointを作成しindexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	return joint.index;

}

void CharacterAnimator::ApplyAnimation()
{
	for (Joint& joint : skeleton_.joints) {
		//対象のJointにAnimationがあれば、値の適応を行う。下記のif文はC++17から可能になった初期化月if文
		if (auto it = animation_.nodeAnimations.find(joint.name); it != animation_.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate, animationTime_);
			joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate, animationTime_);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale, animationTime_);
		}
	}
}

void CharacterAnimator::SkeletonUpdate()
{
	for (Joint& joint : skeleton_.joints) {
		joint.localMatrix = MakeAffineMatrix
		(joint.transform.translate, joint.transform.scale, joint.transform.rotate);
		if (joint.parent) {//親がいれば行列をかける
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {//親がいあないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void CharacterAnimator::SkinClusterUpdate()
{
	for (size_t jointIndex = 0; jointIndex < skeleton_.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster_.inverseBindPoseMatrices[jointIndex] * skeleton_.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			TransposeMatrix4x4(skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix);
	}
}




void CharacterAnimator::noUpdate(Matrix4x4 viewMatrix)
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

void CharacterAnimator::Update(Matrix4x4 viewMatrix)
{

	animationTime_ += 1.0f / 60.0f;//時間を進める
	animationTime_ = std::fmod(animationTime_, animation_.duration);

	ApplyAnimation();

	SkeletonUpdate();
	SkinClusterUpdate();

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

void CharacterAnimator::CreateSkinCluster()
{


	//palette用のResourceを確保
	skinCluster_.paletteResource = GraphicsDevice::CreateBufferResource(sizeof(WellForGPU) * skeleton_.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster_.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster_.mappedPalette = { mappedPalette,skeleton_.joints.size() };
	skinCluster_.paletteSrvHandle.first = GetCPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());
	skinCluster_.paletteSrvHandle.second = GetGPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());

	//palette用のSRVを作成。StructerBufferでアクセスできるようにする
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton_.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device->CreateShaderResourceView(skinCluster_.paletteResource.Get(), &paletteSrvDesc, skinCluster_.paletteSrvHandle.first);

	//influence用のResourceを確保。頂点ごとにinfluence情報をついかできるようにする
	skinCluster_.influenceResource = GraphicsDevice::CreateBufferResource(sizeof(VertexInfluence) * modelData_.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster_.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData_.vertices.size());//0梅wightを0にしておく
	skinCluster_.mappedInfluence = { mappedInfluence,modelData_.vertices.size() };

	//Influence用のVBVを作成
	skinCluster_.influenceBufferView.BufferLocation = skinCluster_.influenceResource->GetGPUVirtualAddress();
	skinCluster_.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData_.vertices.size());
	skinCluster_.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	skinCluster_.inverseBindPoseMatrices.resize(skeleton_.joints.size());
	// std::generate(...) の代わりに for 文で初期化
	//std::generate(skinCluster_.inverseBindPoseMatrices.begin(), skinCluster_.inverseBindPoseMatrices.end(), IdentityMatrix());
	for (size_t i = 0; i < skinCluster_.inverseBindPoseMatrices.size(); ++i) {
		skinCluster_.inverseBindPoseMatrices[i] = IdentityMatrix();
	}

	for (const auto& JointWeight : modelData_.skinClusterData) {//modelのSkinClusterの情報解析
		auto it = skeleton_.jointMap.find(JointWeight.first);
		if (it == skeleton_.jointMap.end()) {
			continue;
		}
		//(*it).secondにはjointのindexが入っているので、外套のindexのinverseBindPoseMatrixを代入
		skinCluster_.inverseBindPoseMatrices[(*it).second] = JointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWight : JointWeight.second.vertexWeights) {
			auto& currentInfluece = skinCluster_.mappedInfluence[vertexWight.VertexIndex];//外套のvertexIndexのinfluence情報を参照しておく
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {//空いているところに入れる
				if (currentInfluece.wights[index] == 0.0f) {//wight==0が空いている状態なので、そに場所にwightとjointのindexを代入
					currentInfluece.wights[index] = vertexWight.weight;
					currentInfluece.jointIndices[index] = (*it).second;
					break;
				}
			}
		}

	}

}

