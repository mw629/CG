#pragma once
#include <cmath>


class Vector3Int
{
public:
	int x = 0;
	int y = 0;
	int z = 0;


	Vector3Int operator+(const Vector3Int& other) const {
		return { x + other.x, y + other.y, z + other.z };
	}
	Vector3Int operator-(const Vector3Int& other) const {
		return { x - other.x, y - other.y , z - other.z };
	}

	Vector3Int operator-() const {
		return Vector3Int(-x, -y, -z);
	}

	Vector3Int operator*(const Vector3Int& other)const {
		return Vector3Int(x * other.x, y * other.y, z * other.z);
	}

	Vector3Int operator*(int scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}

	Vector3Int operator/(const Vector3Int& other) const {
		return { x / other.x, y / other.y ,z / other.z };
	}

	Vector3Int& operator=(const Vector3Int& other) {
		x = other.x; y = other.y; z = other.z; return *this;
	}
	Vector3Int& operator+=(const Vector3Int& other) {
		x += other.x; y += other.y; z += other.z; return *this;
	}
	Vector3Int& operator-=(const Vector3Int& other) {
		x -= other.x; y -= other.y;  z -= other.z; return *this;
	}
	Vector3Int& operator*=(const Vector3Int& other) {
		x *= other.x; y *= other.y; z *= other.z; return *this;
	}
	Vector3Int& operator/=(const Vector3Int& other) {
		x /= other.x; y /= other.y; z /= other.z; return *this;
	}

	// 長さ
	float Length() const {
		return static_cast<float>(std::sqrt(x * x + y * y));
	}

	// 正規化
	Vector3Int Normalization() const {
		int len = (int)Length();
		if (len == 0) return Vector3Int(0, 0, 0); // 0除算防止
		return Vector3Int(x / len, y / len, z / len);
	}

	// 内積
	int Dot(const Vector3Int& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	// 外積
	Vector3Int Cross(const Vector3Int& other) const {
		return { y * other.z - z * other.y,z * other.x - x * other.z,x * other.y - y * other.x };
	}

	// 距離
	static float Distance(const Vector3Int& v1, const Vector3Int& v2) {
		return (v1 - v2).Length();
	}



};
