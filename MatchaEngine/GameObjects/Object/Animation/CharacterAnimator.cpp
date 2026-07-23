#include "CharacterAnimator.h"
#include "Calculation.h"
#include "DescriptorHeap.h"
#include <Load.h>
#include <algorithm>
#include "../../../Graphics/Render/LineRenderer.h"
#include <Sphere.h>

namespace {
	ID3D12Device* device;
	DescriptorHeap* descriptorHeap; 
}

CharacterAnimator::~CharacterAnimator()
{
	if (boneRenderer_) {
		delete boneRenderer_;
		boneRenderer_ = nullptr;
	}
}

CharacterAnimator::CharacterAnimator()
{
}

void CharacterAnimator::SetData(ID3D12Device* SetDevice, DescriptorHeap* SetDescriptorHeap)
{
	device = SetDevice;
	descriptorHeap = SetDescriptorHeap;
}

void CharacterAnimator::Initialize(ModelData modelData, const std::string& directoryPath, const std::string& filename)
{
	modelData_ = modelData;
	animation_ = LoadAnimationFile(directoryPath, filename);
	if (!animation_.animationClips.empty()) {
		currentAnimationName_ = animation_.animationClips.begin()->first;
	}
	textureSrvHandleGPU_ = texture->TextureData(modelData_.textureIndex);

	localMatrix_ = modelData_.rootNode.localMatrix;

	skeleton_ = CreateSkeleton(modelData_.rootNode);
	CreateSkinCluster();
	
	jointSpheres_.resize(skeleton_.joints.size());
	for (size_t i = 0; i < skeleton_.joints.size(); ++i) {
		jointSpheres_[i] = std::make_shared<Sphere>();
		jointSpheres_[i]->SetMaxInstanceCount(1);
		jointSpheres_[i]->SetSubdivision(8);
		jointSpheres_[i]->Initialize(modelData_.textureIndex);
		jointSpheres_[i]->SetName(skeleton_.joints[i].name);
		if (auto mat = jointSpheres_[i]->GetComponent<MaterialComponent>()) {
			mat->GetMaterialFactory()->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
			mat->SetShader("WireFrameShaderNoDepth");
		}
	}

	AddComponent<MaterialComponent>();
	auto matComp = GetComponent<MaterialComponent>();
	if (matComp && !modelData_.material.textureDilePath.empty()) {
		matComp->SetTexturePath(modelData_.material.textureDilePath);
	}
	
	subMeshMaterials_.clear();
	for (const auto& subMesh : modelData_.subMeshes) {
		ModelSubMeshMaterial mat;
		if (subMesh.textureIndex != -1) {
			mat.textureSrvHandleGPU = texture->TextureData(subMesh.textureIndex);
		} else {
			mat.textureSrvHandleGPU = textureSrvHandleGPU_;
		}
		mat.materialFactory = std::make_unique<MaterialFactory>();
		mat.materialFactory->CreateMartial(false, 0.0f);
		subMeshMaterials_.push_back(std::move(mat));
	}
	CreateObject();

	SetShader(AnimationObj);

	boneRenderer_ = new LineRenderer();
	boneRenderer_->Initialize();
}

void CharacterAnimator::LoadAdditionalAnimation(const std::string& directoryPath, const std::string& filename, const std::string& overrideName)
{
	Animation newAnim = LoadAnimationFile(directoryPath, filename);
	for (const auto& pair : newAnim.animationClips) {
		std::string name = overrideName.empty() ? pair.first : overrideName;
		animation_.animationClips[name] = pair.second;
	}
}




void CharacterAnimator::SettingWvp(Matrix4x4 viewMatrix) {
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth_) / float(kClientHeight_), 0.1f, 10000.0f);

	if (isInstancing_ && !instancingTransforms_.empty()) {
		int count = std::min(maxInstanceCount_, static_cast<int>(instancingTransforms_.size()));
		for (int i = 0; i < count; ++i) {
			Matrix4x4 worldMatrix = MakeAffineMatrix(instancingTransforms_[i].translate, instancingTransforms_[i].scale, instancingTransforms_[i].rotate);
			Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatrix));
			Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldMatrix));

			wvpData_[s_wvpIndex][i].WVP = worldViewProjectionMatrix;
			wvpData_[s_wvpIndex][i].World = worldMatrix; // localMatrix_ is handled inside Animation if needed, or maybe we don't multiply localMatrix_ here if skeleton handles it? Wait, localMatrix_ was multiplied here in the original code. Let's keep it if noUpdate uses it, but noUpdate sets it to localMatrix_ * worldMatrix. Wait, for Skinning, world position is just worldMatrix.
			wvpData_[s_wvpIndex][i].WorldInverseTranspose = worldInverseTranspose;
			wvpData_[s_wvpIndex][i].numBones = static_cast<uint32_t>(skeleton_.joints.size());
		}
	} else {
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
		Matrix4x4 worldViewProjectionMatrix = MultiplyMatrix4x4(worldMatrix, MultiplyMatrix4x4(viewMatrix, projectionMatrix));
		Matrix4x4 worldInverseTranspose = TransposeMatrix4x4(Inverse(worldMatrix));

		wvpData_[s_wvpIndex][0].WVP = worldViewProjectionMatrix;
		wvpData_[s_wvpIndex][0].World = worldMatrix;
		wvpData_[s_wvpIndex][0].WorldInverseTranspose = worldInverseTranspose;
		wvpData_[s_wvpIndex][0].numBones = static_cast<uint32_t>(skeleton_.joints.size());
	}
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

void CharacterAnimator::ApplyAnimation(float time)
{
	if (animation_.animationClips.find(currentAnimationName_) == animation_.animationClips.end()) return;
	const AnimationClip& clip = animation_.animationClips.at(currentAnimationName_);

	for (Joint& joint : skeleton_.joints) {
		//対象のJointにAnimationがあれば、値の適応を行う。下記のif文はC++17から可能になった初期化月if文
		if (auto it = clip.AnimationNodes.find(joint.name); it != clip.AnimationNodes.end()) {
			const AnimationNode& rootAnimationNode = (*it).second;
			joint.transform.translate = CalculateValue(rootAnimationNode.translate, time);
			joint.transform.rotate = CalculateValue(rootAnimationNode.rotate, time);
			joint.transform.scale = CalculateValue(rootAnimationNode.scale, time);
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

void CharacterAnimator::SkinClusterUpdate(int instanceIndex)
{
	size_t offset = instanceIndex * skeleton_.joints.size();
	for (size_t jointIndex = 0; jointIndex < skeleton_.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());
		skinCluster_.mappedPalette[offset + jointIndex].skeletonSpaceMatrix =
			skinCluster_.inverseBindPoseMatrices[jointIndex] * skeleton_.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster_.mappedPalette[offset + jointIndex].skeletonSpaceInverseTransposeMatrix =
			TransposeMatrix4x4(skinCluster_.mappedPalette[offset + jointIndex].skeletonSpaceMatrix);
	}
}




void CharacterAnimator::noUpdate(Matrix4x4 viewMatrix)
{
	if (animation_.animationClips.find(currentAnimationName_) == animation_.animationClips.end()) {
		SettingWvp(viewMatrix);
		return;
	}
	AnimationClip& clip = animation_.animationClips.at(currentAnimationName_);

	animationTime_ += 1.0f / 60.0f;//時間を進める
	animationTime_ = std::fmod(animationTime_, clip.duration);//リピート再生
	AnimationNode& rootAnimationNode = clip.AnimationNodes[modelData_.rootNode.name];

	Vector3 translate = CalculateValue(rootAnimationNode.translate, animationTime_);
	Quaternion rotate = CalculateValue(rootAnimationNode.rotate, animationTime_);

	Vector3 scale = CalculateValue(rootAnimationNode.scale, animationTime_);
	localMatrix_ = MakeAffineMatrix(translate, scale, rotate);

	SettingWvp(viewMatrix);
}

void CharacterAnimator::Update(Matrix4x4 viewMatrix)
{
	float currentDuration = GetDuration();
	if (currentDuration > 0.0f) {
		if (isInstancing_ && !instancingTransforms_.empty()) {
			int count = std::min(maxInstanceCount_, static_cast<int>(instancingTransforms_.size()));
			for (int i = 0; i < count; ++i) {
				instancingAnimationTimes_[i] += 1.0f / 60.0f;//時間を進める
				instancingAnimationTimes_[i] = std::fmod(instancingAnimationTimes_[i], currentDuration);

				ApplyAnimation(instancingAnimationTimes_[i]);
				SkeletonUpdate();
				SkinClusterUpdate(i);
			}
		} else {
			animationTime_ += 1.0f / 60.0f;//時間を進める
			animationTime_ = std::fmod(animationTime_, currentDuration);

			ApplyAnimation(animationTime_);
			SkeletonUpdate();
			SkinClusterUpdate(0);
		}
	}

	SettingWvp(viewMatrix);
	UpdateBoneRenderer();
	
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	if (isInstancing_ && !instancingTransforms_.empty()) {
		worldMatrix = MakeAffineMatrix(instancingTransforms_[0].translate, instancingTransforms_[0].scale, instancingTransforms_[0].rotate);
	}
	for (size_t i = 0; i < skeleton_.joints.size(); ++i) {
		if (i < jointSpheres_.size() && jointSpheres_[i]) {
			Matrix4x4 currentJointMat = skeleton_.joints[i].skeletonSpaceMatrix * worldMatrix;
			Transform t = DecomposeMatrix(currentJointMat);
			t.scale = { 0.01f, 0.01f, 0.01f };
			jointSpheres_[i]->SetTransform(t);
			jointSpheres_[i]->SettingWvp(viewMatrix);
		}
	}
}

void CharacterAnimator::UpdateWithDelta(Matrix4x4 viewMatrix, float deltaAnimationTime)
{
	float currentDuration = GetDuration();
	if (currentDuration > 0.0f) {
		if (isInstancing_ && !instancingTransforms_.empty()) {
			int count = std::min(maxInstanceCount_, static_cast<int>(instancingTransforms_.size()));
			for (int i = 0; i < count; ++i) {
				instancingAnimationTimes_[i] += deltaAnimationTime;
				instancingAnimationTimes_[i] = std::fmod(instancingAnimationTimes_[i], currentDuration);
				if (instancingAnimationTimes_[i] < 0.0f) {
					instancingAnimationTimes_[i] += currentDuration;
				}

				ApplyAnimation(instancingAnimationTimes_[i]);
				SkeletonUpdate();
				SkinClusterUpdate(i);
			}
		} else {
			animationTime_ += deltaAnimationTime;
			animationTime_ = std::fmod(animationTime_, currentDuration);
			if (animationTime_ < 0.0f) {
				animationTime_ += currentDuration;
			}

			ApplyAnimation(animationTime_);
			SkeletonUpdate();
			SkinClusterUpdate(0);
		}
	}

	SettingWvp(viewMatrix);
	UpdateBoneRenderer();

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
	if (isInstancing_ && !instancingTransforms_.empty()) {
		worldMatrix = MakeAffineMatrix(instancingTransforms_[0].translate, instancingTransforms_[0].scale, instancingTransforms_[0].rotate);
	}
	for (size_t i = 0; i < skeleton_.joints.size(); ++i) {
		if (i < jointSpheres_.size() && jointSpheres_[i]) {
			Matrix4x4 currentJointMat = skeleton_.joints[i].skeletonSpaceMatrix * worldMatrix;
			Transform t = DecomposeMatrix(currentJointMat);
			t.scale = { 0.01f, 0.01f, 0.01f };
			jointSpheres_[i]->SetTransform(t);
			jointSpheres_[i]->SettingWvp(viewMatrix);
		}
	}
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

void CharacterAnimator::UpdateBoneRenderer()
{
	if (!isVisibleBones_ || !boneRenderer_) return;

	if (isInstancing_ && !instancingTransforms_.empty()) {
		int count = std::min(maxInstanceCount_, static_cast<int>(instancingTransforms_.size()));
		for (int i = 0; i < count; ++i) {
			Matrix4x4 worldMatrix = MakeAffineMatrix(instancingTransforms_[i].translate, instancingTransforms_[i].scale, instancingTransforms_[i].rotate);

			for (const Joint& joint : skeleton_.joints) {
				if (joint.parent) {
					Matrix4x4 currentJointMat = joint.skeletonSpaceMatrix * worldMatrix;
					Matrix4x4 parentJointMat = skeleton_.joints[*joint.parent].skeletonSpaceMatrix * worldMatrix;

					Vector3 startPos = { currentJointMat.m[3][0], currentJointMat.m[3][1], currentJointMat.m[3][2] };
					Vector3 endPos = { parentJointMat.m[3][0], parentJointMat.m[3][1], parentJointMat.m[3][2] };
					
					boneRenderer_->AddLine(startPos, endPos, { 1.0f, 0.0f, 1.0f, 1.0f }); // 派手なマゼンタ色
				}
			}
		}
	} else {
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);

		for (const Joint& joint : skeleton_.joints) {
			if (joint.parent) {
				Matrix4x4 currentJointMat = joint.skeletonSpaceMatrix * worldMatrix;
				Matrix4x4 parentJointMat = skeleton_.joints[*joint.parent].skeletonSpaceMatrix * worldMatrix;

				Vector3 startPos = { currentJointMat.m[3][0], currentJointMat.m[3][1], currentJointMat.m[3][2] };
				Vector3 endPos = { parentJointMat.m[3][0], parentJointMat.m[3][1], parentJointMat.m[3][2] };
				
				boneRenderer_->AddLine(startPos, endPos, { 1.0f, 0.0f, 1.0f, 1.0f }); // 派手なマゼンタ色
			}
		}
	}
}

void CharacterAnimator::CreateSkinCluster()
{


	//palette用のResourceを確保
	skinCluster_.paletteResource = GraphicsDevice::CreateBufferResource(sizeof(WellForGPU) * skeleton_.joints.size() * maxInstanceCount_);
	WellForGPU* mappedPalette = nullptr;
	skinCluster_.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster_.mappedPalette = { mappedPalette, skeleton_.joints.size() * maxInstanceCount_ };
	skinCluster_.paletteSrvHandle.first = GetCPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());
	skinCluster_.paletteSrvHandle.second = GetGPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());

	//palette用のSRVを作成。StructerBufferでアクセスできるようにする
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton_.joints.size() * maxInstanceCount_);
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	device->CreateShaderResourceView(skinCluster_.paletteResource.Get(), &paletteSrvDesc, skinCluster_.paletteSrvHandle.first);

	//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	skinCluster_.inverseBindPoseMatrices.resize(skeleton_.joints.size());
	for (size_t i = 0; i < skinCluster_.inverseBindPoseMatrices.size(); ++i) {
		skinCluster_.inverseBindPoseMatrices[i] = IdentityMatrix();
	}

	// global skin cluster data just in case some root nodes are missing
	for (const auto& JointWeight : modelData_.skinClusterData) {
		auto it = skeleton_.jointMap.find(JointWeight.first);
		if (it != skeleton_.jointMap.end()) {
			skinCluster_.inverseBindPoseMatrices[(*it).second] = JointWeight.second.inverseBindPoseMatrix;
		}
	}

	subMeshInfluenceResources_.clear();
	subMeshInfluenceBufferViews_.clear();

	for (size_t meshIndex = 0; meshIndex < modelData_.subMeshes.size(); ++meshIndex) {
		const auto& subMesh = modelData_.subMeshes[meshIndex];
		
		Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource = GraphicsDevice::CreateBufferResource(sizeof(VertexInfluence) * subMesh.mesh.vertexSize);
		VertexInfluence* mappedInfluence = nullptr;
		influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * subMesh.mesh.vertexSize);
		std::span<VertexInfluence> mappedSpan = { mappedInfluence, static_cast<std::span<VertexInfluence>::size_type>(subMesh.mesh.vertexSize) };

		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		influenceBufferView.BufferLocation = influenceResource->GetGPUVirtualAddress();
		influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * subMesh.mesh.vertexSize);
		influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

		for (const auto& JointWeight : subMesh.skinClusterData) {
			auto it = skeleton_.jointMap.find(JointWeight.first);
			if (it == skeleton_.jointMap.end()) {
				continue;
			}
			skinCluster_.inverseBindPoseMatrices[(*it).second] = JointWeight.second.inverseBindPoseMatrix;
			
			for (const auto& vertexWight : JointWeight.second.vertexWeights) {
				auto& currentInfluece = mappedSpan[vertexWight.VertexIndex];
				for (uint32_t index = 0; index < kNumMaxInfluence; ++index) {
					if (currentInfluece.wights[index] == 0.0f) {
						currentInfluece.wights[index] = vertexWight.weight;
						currentInfluece.jointIndices[index] = (*it).second;
						break;
					}
				}
			}
		}

		subMeshInfluenceResources_.push_back(influenceResource);
		subMeshInfluenceBufferViews_.push_back(influenceBufferView);

		// GPU Skinning output resource (UAV + VB compatibility)
		Microsoft::WRL::ComPtr<ID3D12Resource> skinnedResource = GraphicsDevice::CreateUAVBufferResource(sizeof(VertexData) * subMesh.mesh.vertexSize);
		D3D12_VERTEX_BUFFER_VIEW skinnedBufferView{};
		skinnedBufferView.BufferLocation = skinnedResource->GetGPUVirtualAddress();
		skinnedBufferView.SizeInBytes = UINT(sizeof(VertexData) * subMesh.mesh.vertexSize);
		skinnedBufferView.StrideInBytes = sizeof(VertexData);

		subMeshSkinnedResources_.push_back(skinnedResource);
		subMeshSkinnedBufferViews_.push_back(skinnedBufferView);

		// Allocate and create Input Vertex SRV (t1)
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> inputVertexSrvHandle;
		inputVertexSrvHandle.first = GetCPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());
		inputVertexSrvHandle.second = GetGPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());

		D3D12_SHADER_RESOURCE_VIEW_DESC inputVertexSrvDesc{};
		inputVertexSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		inputVertexSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		inputVertexSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		inputVertexSrvDesc.Buffer.FirstElement = 0;
		inputVertexSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		inputVertexSrvDesc.Buffer.NumElements = UINT(subMesh.mesh.vertexSize);
		inputVertexSrvDesc.Buffer.StructureByteStride = sizeof(VertexData);
		device->CreateShaderResourceView(subMesh.mesh.vertexResource.Get(), &inputVertexSrvDesc, inputVertexSrvHandle.first);

		subMeshInputVertexSrvHandles_.push_back(inputVertexSrvHandle);

		// Allocate and create Influence SRV (t2)
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> influenceSrvHandle;
		influenceSrvHandle.first = GetCPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());
		influenceSrvHandle.second = GetGPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());

		D3D12_SHADER_RESOURCE_VIEW_DESC influenceSrvDesc{};
		influenceSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
		influenceSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		influenceSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		influenceSrvDesc.Buffer.FirstElement = 0;
		influenceSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		influenceSrvDesc.Buffer.NumElements = UINT(subMesh.mesh.vertexSize);
		influenceSrvDesc.Buffer.StructureByteStride = sizeof(VertexInfluence);
		device->CreateShaderResourceView(influenceResource.Get(), &influenceSrvDesc, influenceSrvHandle.first);

		subMeshInfluenceSrvHandles_.push_back(influenceSrvHandle);

		// Allocate and create Output Vertex UAV (u0)
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> outputVertexUavHandle;
		outputVertexUavHandle.first = GetCPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());
		outputVertexUavHandle.second = GetGPUDescriptorHandle(descriptorHeap->GetSrvDescriptorHeap(), descriptorHeap->GetDescriptorSizeSRV());

		D3D12_UNORDERED_ACCESS_VIEW_DESC outputVertexUavDesc{};
		outputVertexUavDesc.Format = DXGI_FORMAT_UNKNOWN;
		outputVertexUavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		outputVertexUavDesc.Buffer.FirstElement = 0;
		outputVertexUavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		outputVertexUavDesc.Buffer.NumElements = UINT(subMesh.mesh.vertexSize);
		outputVertexUavDesc.Buffer.StructureByteStride = sizeof(VertexData);
		device->CreateUnorderedAccessView(skinnedResource.Get(), nullptr, &outputVertexUavDesc, outputVertexUavHandle.first);

		subMeshOutputVertexUavHandles_.push_back(outputVertexUavHandle);

		// Create Constant Buffer for Skinning Information (b0)
		Microsoft::WRL::ComPtr<ID3D12Resource> skinningInfoResource = GraphicsDevice::CreateBufferResource(sizeof(uint32_t));
		uint32_t* mappedInfo = nullptr;
		skinningInfoResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfo));
		mappedInfo[0] = static_cast<uint32_t>(subMesh.mesh.vertexSize);
		skinningInfoResource->Unmap(0, nullptr);

		subMeshSkinningInfoResources_.push_back(skinningInfoResource);
	}

	// For backward compatibility / original mesh
	if (!subMeshInfluenceResources_.empty()) {
		skinCluster_.influenceResource = subMeshInfluenceResources_[0];
		skinCluster_.influenceBufferView = subMeshInfluenceBufferViews_[0];
	}
}

Transform CharacterAnimator::GetBoneTransform(BoneType type)
{
	if (boneTypeMap_.find(type) != boneTypeMap_.end()) {
		std::string boneName = boneTypeMap_[type];
		auto it = skeleton_.jointMap.find(boneName);
		if (it != skeleton_.jointMap.end()) {
			int32_t index = it->second;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.translate, transform_.scale, transform_.rotate);
			Matrix4x4 boneWorldMatrix = MultiplyMatrix4x4(skeleton_.joints[index].skeletonSpaceMatrix, worldMatrix);
			return DecomposeMatrix(boneWorldMatrix);
		}
	}
	return Transform();
}
