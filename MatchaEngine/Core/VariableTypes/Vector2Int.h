#pragma once
#include <cmath>


class VectorInt2Int
{
public:
	int x = 0;
	int y = 0;



	VectorInt2Int operator+(const VectorInt2Int& other) const {
		return { x + other.x, y + other.y };
	}
	VectorInt2Int operator-(const VectorInt2Int& other) const {
		return { x - other.x, y - other.y };
	}

	VectorInt2Int operator-() const {
		return VectorInt2Int(-x, -y);
	}

	VectorInt2Int operator*(const VectorInt2Int& other)const {
		return VectorInt2Int(x * other.x, y * other.y);
	}

	VectorInt2Int operator*(int scalar) const {
		return { x * scalar, y * scalar };
	}

	VectorInt2Int operator/(const VectorInt2Int& other) const {
		return { x / other.x, y / other.y };
	}

	VectorInt2Int& operator=(const VectorInt2Int& other) {
		x = other.x; y = other.y; return *this;
	}
	VectorInt2Int& operator+=(const VectorInt2Int& other) {
		x += other.x; y += other.y; return *this;
	}
	VectorInt2Int& operator-=(const VectorInt2Int& other) {
		x -= other.x; y -= other.y;  return *this;
	}
	VectorInt2Int& operator*=(const VectorInt2Int& other) {
		x *= other.x; y *= other.y; return *this;
	}
	VectorInt2Int& operator/=(const VectorInt2Int& other) {
		x /= other.x; y /= other.y; return *this;
	}


	// 長さ
	float Length() const {
		return static_cast<float>(std::sqrt(x * x + y * y));
	}

	// 正規化
	VectorInt2Int Normalization() const {
		int len = (int)Length();
		if (len == 0) return VectorInt2Int(0, 0); // 0除算防止
		return VectorInt2Int(x / len, y / len);
	}

	// 内積
	int Dot(const VectorInt2Int& other) const {
		return x * other.x + y * other.y;
	}

	// 外積
	int Cross(const VectorInt2Int& other) const {
		return x * other.y - y * other.x;
	}

	// 距離
	static float Distance(const VectorInt2Int& v1, const VectorInt2Int& v2) {
		return (v1 - v2).Length();
	}



};

