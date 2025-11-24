#pragma once
#include <cstdint>
#include <vector>
#include <string>

#include "VariableTypes/Vector2.h"
#include "VariableTypes/Vector2Int.h"
#include "VariableTypes/Vector3.h"
#include "VariableTypes/Vector3Int.h"

#include "VariableTypes/Matrix4x4.h"

///構造体///

struct Vector4 {
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
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
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
};

struct DirectionalLight {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
};

struct MaterialData
{
	std::string textureDilePath;
};

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
	int textureIndex;
};

struct Segment {
	Vector3 origin; //始点
	Vector3 diff;//終点への差分
};



