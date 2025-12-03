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

struct DirectionalLight {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
};

struct MaterialData
{
	std::string textureDilePath;
};

struct Node {
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
	int textureIndex;
	Node rootNode;
};

struct Segment {
	Vector3 origin; //始点
	Vector3 diff;//終点への差分
};



