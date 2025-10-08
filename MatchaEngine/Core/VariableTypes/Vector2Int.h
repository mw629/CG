#pragma once
#include <cmath>


class Vector2Int
{
public:
	int x = 0;
	int y = 0;

	Vector2Int operator+(const Vector2Int& other) const {
		return { x + other.x, y + other.y };
	}
	Vector2Int operator-(const Vector2Int& other) const {
		return { x - other.x, y - other.y };
	}

	Vector2Int operator-() const {
		return Vector2Int(-x, -y);
	}

	Vector2Int operator*(const Vector2Int& other)const {
		return Vector2Int(x * other.x, y * other.y);
	}

	Vector2Int operator*(int scalar) const {
		return { x * scalar, y * scalar };
	}

	Vector2Int operator/(const Vector2Int& other) const {
		return { x / other.x, y / other.y };
	}

	Vector2Int& operator=(const Vector2Int& other) {
		x = other.x; y = other.y; return *this;
	}
	Vector2Int& operator+=(const Vector2Int& other) {
		x += other.x; y += other.y; return *this;
	}
	Vector2Int& operator-=(const Vector2Int& other) {
		x -= other.x; y -= other.y;  return *this;
	}
	Vector2Int& operator*=(const Vector2Int& other) {
		x *= other.x; y *= other.y; return *this;
	}
	Vector2Int& operator/=(const Vector2Int& other) {
		x /= other.x; y /= other.y; return *this;
	}


	// 長さ
	float Length() const {
		return static_cast<float>(std::sqrt(x * x + y * y));
	}

	// 正規化
	Vector2Int Normalization() const {
		int len = (int)Length();
		if (len == 0) return Vector2Int(0, 0); // 0除算防止
		return Vector2Int(x / len, y / len);
	}

	// 内積
	int Dot(const Vector2Int& other) const {
		return x * other.x + y * other.y;
	}

	// 外積
	int Cross(const Vector2Int& other) const {
		return x * other.y - y * other.x;
	}

	// 距離
	static float Distance(const Vector2Int& v1, const Vector2Int& v2) {
		return (v1 - v2).Length();
	}



};

