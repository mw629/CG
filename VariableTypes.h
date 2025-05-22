#pragma once
#include <cstdint>


///構造体///

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Vector3 {
	float x;
	float y;
	float z;
	

	Vector3 operator+(const Vector3& other) { return { x + other.x, y + other.y,z + other.z }; }
	Vector3 operator-(const Vector3& other) { return { x - other.x, y - other.y,z - other.z }; }
	Vector3 operator*(float other) const { return { x * other, y * other,z * other }; }
	Vector3 operator/(const Vector3& other) { return { x / other.x, y / other.y ,z / other.z }; }
	Vector3 operator=(const Vector3& other) {x = other.x;y = other.y;z = other.z; return *this;}
	Vector3 operator+=(const Vector3& other) { return { x += other.x, y += other.y,z += other.z }; }
	Vector3 operator-=(const Vector3& other) { return { x -= other.x, y -= other.y,z -= other.z }; }
	Vector3 operator*=(const Vector3& other) { return { x *= other.x, y *= other.y,z *= other.z }; }
	Vector3 operator/=(const Vector3& other) { return { x /= other.x, y /= other.y,z /= other.z }; }
};
struct Vector2
{
	float x;
	float y;
};

struct Matrix4x4 {
	float m[4][4] = {
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
	};
};

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material
{
	Vector4 color;
	int32_t endbleLighting;
};

struct DirectionalLight {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
};