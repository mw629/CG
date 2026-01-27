#pragma once
#include <VariableTypes.h>
#include "ObjectBase.h"
#include <Model.h>
#include "Line.h"

class CharacterAnimator :public ObjectBase
{
private:

	ModelData modelData_{};

	Animation animation_;
	float animationTime_ = 0.0f;

	Skeleton skeleton_;
	SkinCluster skinCluster_;

	Matrix4x4 localMatrix_;

	// スケルトン描画用
	std::vector<std::unique_ptr<Line>> skeletonLines_;
	bool isDrawSkeleton_ = true;

public:

	~CharacterAnimator()override;
	CharacterAnimator();

	static void SetData(ID3D12Device* SetDevice, DescriptorHeap* SetDescriptorHeap);

	void Initialize(const std::string& directoryPath, const std::string& filename);

	void CreateVertexData()override;

	void CreateIndexResource()override;

	void SettingWvp(Matrix4x4 viewMatrix) override;

	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node,
		const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	void ApplyAnimation();
	
	void SkeletonUpdate();
	void SkinClusterUpdate();

	void noUpdate(Matrix4x4 viewMatrix);
	void Update(Matrix4x4 viewMatrix);

	ModelData GetModelData() { return modelData_; }
	int GetIndexSize()override { return modelData_.indices.size(); }

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframe, float time);
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframe, float time);

	void CreateSkinCluster();

	// 追加: インフルエンス用 VBV を取得
	D3D12_VERTEX_BUFFER_VIEW* GetInfluenceBufferView() { return &skinCluster_.influenceBufferView; }

	// 追加: スキンパレット SRV の GPU ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetPaletteSrvHandleGPU() const { return skinCluster_.paletteSrvHandle.second; }
};

