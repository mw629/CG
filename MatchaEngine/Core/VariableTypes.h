#pragma once
#include <cstdint>
#include <vector>
#include <string>

#include "VariableTypes/Vector2.h"
#include "VariableTypes/Vector2Int.h"
#include "VariableTypes/Vector3.h"
#include "VariableTypes/Vector3Int.h"
#include "VariableTypes/Matrix4x4.h"
#include <map>
#include <optional>


///構造体///

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};


struct Quaternion {
	float x;
	float y;
	float z;
	float w;
};

struct PolarCoordinates {
	Vector3 offset;
	float radius;
	float theta;
	float phi;
};




/// <summary>
/// 3Dオブジェクトのスケール、回転、平行移動を表す構造体です。
/// </summary>
struct Transform
{
	Vector3 scale = { 1.0f,1.0f,1.0f };
	Vector3 rotate = { 0.0f,0.0f,0.0f };
	Vector3 translate = { 0.0f,0.0f,0.0f };
};

struct QuaternionTransform
{
	Vector3 scale = { 1.0f,1.0f,1.0f };
	Quaternion rotate = { 0.0f,0.0f,0.0f };
	Vector3 translate = { 0.0f,0.0f,0.0f };
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct CameraForGPU {
	Vector3 worldPosition;
};

struct LineTransformationMatrix
{
	Matrix4x4 WVP;
};


struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;

};

struct LineVertexData {
	Vector3 position;
	Vector4 color;
};

struct Material
{
	Vector4 color;
	int32_t endbleLighting;
	float paddinmg[3];
	Matrix4x4 uvTransform;
	float shininess;
};

struct DirectionalLightData {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
};

struct PointLightData {
	Vector4 color;//ライトの色
	Vector3 position;//ライトの向き
	float intensity;//輝度
	float radius;//ライトの届く最大距離
	float decay;//減衰率
	float padding[2];
};

struct SpotLightData {
	Vector4 color;//ライトの色
	Vector3 position;//ライトの向き
	float intensity;//輝度
	Vector3 direction;//ライトの向き
	float distance;//ライトの届く最大距離
	float decay;//減衰率
	float cosAngle;//スポットライトの余弦
	float cosFalloffStart;//Falloの開始角度
	float padding[2];
};

struct MaterialData
{
	std::string textureDilePath;
};


struct Node {
	QuaternionTransform transform;

	Matrix4x4 localMatrix = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	std::string name;
	std::vector<Node> children;
};

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
	std::vector<int32_t>indices;
	int textureIndex;
	Node rootNode;
};


//struct KeyframeVector3 {
//	Vector3 value;//キーフレームの値
//	float time;//キーフレームの時間
//};
//
//struct KeyframeQuaternion {
//	Quaternion value;
//	float time;
//};

template<typename tValue>

struct Keyframe {
	float time;
	tValue value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

struct NodeAnimation {
	std::vector<KeyframeVector3> translate;
	std::vector<KeyframeQuaternion> rotate;
	std::vector<KeyframeVector3> scale;

};

struct Animation {
	float duration;//アニメーション全体の尺
	//NodeAnimationの集合。Node名で弾けるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};



struct Segment {
	Vector3 origin; //始点
	Vector3 diff;//終点への差分
};

struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;
	std::string name;//名まえ
	std::vector<int32_t> children;//子ジョイントのインデックス
	int32_t index;//自身のインデックス
	std::optional<int32_t> parent;//親ジョイント
};

struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t> jpintMap;//ジョイントとインデックスの辞書
	std::vector<Joint> joints;
};