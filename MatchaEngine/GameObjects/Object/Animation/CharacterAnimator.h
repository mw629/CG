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

	std::vector<ModelSubMeshMaterial> subMeshMaterials_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> subMeshInfluenceResources_;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> subMeshInfluenceBufferViews_;

public:

	~CharacterAnimator()override;
	CharacterAnimator();

	static void SetData(ID3D12Device* SetDevice, DescriptorHeap* SetDescriptorHeap);

	void Initialize(ModelData modelData, const std::string& directoryPath, const std::string& filename);

	void SettingWvp(Matrix4x4 viewMatrix) override;

	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node,
		const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	void ApplyAnimation(float time);
	
	void SkeletonUpdate();
	void SkinClusterUpdate(int instanceIndex);

	void noUpdate(Matrix4x4 viewMatrix);
	void Update(Matrix4x4 viewMatrix);

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

	// 追加: スキンパレット SRV の GPU ハンドル (インスタンス対応であればGPUのVADDRになるがDrawでGetPaletteResourceGPUを呼ぶように変更済)
	ID3D12Resource* GetPaletteResourceGPU() const { return skinCluster_.paletteResource.Get(); }

	void SetVisibleBones(bool visible) { isVisibleBones_ = visible; }
	bool GetVisibleBones() const { return isVisibleBones_; }
	LineRenderer* GetBoneRenderer() const { return boneRenderer_; }

	Mesh GetMesh()override { return modelData_.mesh; }
};

