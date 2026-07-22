#pragma once
#include <VariableTypes.h>
#include "ObjectBase.h"
#include <Model.h>
#include "Line.h"
class LineRenderer;

class CharacterAnimator :public ObjectBase
{
private:

	ModelData modelData_{};

	Animation animation_;
	float animationTime_ = 0.0f;

	Skeleton skeleton_;
	SkinCluster skinCluster_;

	Matrix4x4 localMatrix_;

	bool isVisibleBones_ = false;
	LineRenderer* boneRenderer_ = nullptr;
	std::vector<std::shared_ptr<class Sphere>> jointSpheres_;

	std::vector<ModelSubMeshMaterial> subMeshMaterials_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> subMeshInfluenceResources_;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> subMeshInfluenceBufferViews_;

	// GPU Skinning (Compute Shader) variables
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> subMeshSkinnedResources_;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> subMeshSkinnedBufferViews_;
	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> subMeshInputVertexSrvHandles_;
	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> subMeshInfluenceSrvHandles_;
	std::vector<std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> subMeshOutputVertexUavHandles_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> subMeshSkinningInfoResources_;

public:

	~CharacterAnimator()override;
	CharacterAnimator();

	static void SetData(ID3D12Device* SetDevice, DescriptorHeap* SetDescriptorHeap);

	void Initialize(ModelData modelData, const std::string& directoryPath, const std::string& filename);

	void SettingWvp(Matrix4x4 viewMatrix) override;

	void SetTexture(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU) {
		ObjectBase::SetTexture(textureSrvHandleGPU);
		for (auto& mat : subMeshMaterials_) {
			mat.textureSrvHandleGPU = textureSrvHandleGPU;
		}
	}

	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node,
		const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	void ApplyAnimation(float time);
	
	void SkeletonUpdate();
	void SkinClusterUpdate(int instanceIndex);

	void noUpdate(Matrix4x4 viewMatrix);
	void Update(Matrix4x4 viewMatrix);
	void UpdateWithDelta(Matrix4x4 viewMatrix, float deltaAnimationTime);

	void SetAnimationTime(float time) { animationTime_ = time; }
	float GetAnimationTime() const { return animationTime_; }
	float GetDuration() const { return animation_.duration; }

	void UpdateBoneRenderer();

	ModelData GetModelData() { return modelData_; }

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

	void CreateSkinCluster();

	std::vector<float> instancingAnimationTimes_;
	void AddInstanceAnimator(Transform transform, float animationTime) {
		AddInstanceTransform(transform);
		instancingAnimationTimes_.push_back(animationTime);
	}
	void ClearInstanceAnimators() {
		ClearInstanceTransforms();
		instancingAnimationTimes_.clear();
	}

	// 追加: インフルエンス用 VBV を取得 (後方互換性)
	D3D12_VERTEX_BUFFER_VIEW* GetInfluenceBufferView() { return &skinCluster_.influenceBufferView; }

	// Multi-mesh support
	const std::vector<ModelSubMeshMaterial>& GetSubMeshMaterials() const { return subMeshMaterials_; }
	D3D12_VERTEX_BUFFER_VIEW* GetSubMeshInfluenceBufferView(size_t index) { return &subMeshInfluenceBufferViews_[index]; }

	// GPU Skinning (Compute Shader) support
	D3D12_VERTEX_BUFFER_VIEW* GetSubMeshSkinnedBufferView(size_t index) { return &subMeshSkinnedBufferViews_[index]; }
	ID3D12Resource* GetSubMeshSkinnedResource(size_t index) { return subMeshSkinnedResources_[index].Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSubMeshInputVertexSrvHandle(size_t index) const { return subMeshInputVertexSrvHandles_[index].second; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSubMeshInfluenceSrvHandle(size_t index) const { return subMeshInfluenceSrvHandles_[index].second; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetSubMeshOutputVertexUavHandle(size_t index) const { return subMeshOutputVertexUavHandles_[index].second; }
	ID3D12Resource* GetSubMeshSkinningInfoResource(size_t index) const { return subMeshSkinningInfoResources_[index].Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetPaletteSrvHandleGPU() const { return skinCluster_.paletteSrvHandle.second; }

	// 追加: スキンパレット SRV の GPU ハンドル (インスタンス対応であればGPUのVADDRになるがDrawでGetPaletteResourceGPUを呼ぶように変更済)
	ID3D12Resource* GetPaletteResourceGPU() const { return skinCluster_.paletteResource.Get(); }

	void SetVisibleBones(bool visible) { isVisibleBones_ = visible; }
	bool GetVisibleBones() const { return isVisibleBones_; }
	LineRenderer* GetBoneRenderer() const { return boneRenderer_; }

	const Skeleton& GetSkeleton() const { return skeleton_; }
	std::shared_ptr<class Sphere> GetJointSphere(int32_t index) const {
		if (index >= 0 && index < jointSpheres_.size()) return jointSpheres_[index];
		return nullptr;
	}

	Mesh GetMesh()override { return modelData_.mesh; }
};

