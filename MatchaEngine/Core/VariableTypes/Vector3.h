#pragma once
#include <cmath>


class Vector3
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;


	Vector3 operator+(const Vector3& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}
	Vector3 operator-(const Vector3& other) const {
		return { x - other.x, y - other.y , z - other.z };
	}

	Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}

	Vector3 operator*(const Vector3& other)const {
		return Vector3(x * other.x, y * other.y, z * other.z);
	}

	Vector3 operator*(float scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3 operator/(const Vector3& other) const {
		return { x / other.x, y / other.y ,z / other.z };
	}

	Vector3& operator=(const Vector3& other) {
		x = other.x; y = other.y; z = other.z; return *this;
	}
	Vector3& operator+=(const Vector3& other) {
		x += other.x; y += other.y; z += other.z; return *this;
	}
	Vector3& operator-=(const Vector3& other) {
		x -= other.x; y -= other.y;  z -= other.z; return *this;
	}
	Vector3& operator*=(const Vector3& other) {
		x *= other.x; y *= other.y; z *= other.z; return *this;
	}
	Vector3& operator/=(const Vector3& other) {
		x /= other.x; y /= other.y; z /= other.z; return *this;
	}

	// 長さ
	float Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	// 正規化
	Vector3 Normalization() const {
		float len = Length();
		if (len == 0) return Vector3(0, 0, 0); // 0除算防止
		return Vector3(x / len, y / len, z / len);
	}

	// 内積
	float Dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	// 外積
	Vector3 Cross(const Vector3& other) const {
		return { y * other.z - z * other.y,z * other.x - x * other.z,x * other.y - y * other.x };
	}

	// 距離
	static float Distance(const Vector3& v1, const Vector3& v2) {
		return (v1 - v2).Length();
	}



};
