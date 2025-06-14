#pragma once
#include <cstdint>
#include <vector>
#include <string>

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


	Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
	Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
	Vector3 operator*(float other) const { return { x * other, y * other, z * other }; }
	Vector3 operator/(const Vector3& other) const { return { x / other.x, y / other.y, z / other.z }; }
	Vector3& operator=(const Vector3& other) { x = other.x; y = other.y; z = other.z; return *this; }
	Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
	Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
	Vector3& operator*=(const Vector3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
	Vector3& operator/=(const Vector3& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }
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

    Matrix4x4 operator+(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] + other.m[i][j];
        return result;
    }

    Matrix4x4 operator-(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] - other.m[i][j];
        return result;
    }

    Matrix4x4 operator*(float scalar) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] * scalar;
        return result;
    }

    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result = {};
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4; ++k)
                    result.m[i][j] += m[i][k] * other.m[k][j];
        return result;
    }

    Matrix4x4& operator=(const Matrix4x4& other) {
        if (this != &other) {
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    m[i][j] = other.m[i][j];
        }
        return *this;
    }

    Matrix4x4& operator+=(const Matrix4x4& other) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] += other.m[i][j];
        return *this;
    }

    Matrix4x4& operator-=(const Matrix4x4& other) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] -= other.m[i][j];
        return *this;
    }

    Matrix4x4& operator*=(float scalar) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] *= scalar;
        return *this;
    }

    Matrix4x4& operator*=(const Matrix4x4& other) {
        *this = *this * other;
        return *this;
    }
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
};

struct Segment {
	Vector3 origin; //始点
	Vector3 diff;//終点への差分
};


