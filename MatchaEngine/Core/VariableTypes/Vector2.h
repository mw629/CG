#pragma once
#include <cmath>


class Vector2
{
public:
	float x = 0.0f;
	float y = 0.0f;


	Vector2 operator+(const Vector2& other) const {
		return { x + other.x, y + other.y };
	}
	Vector2 operator-(const Vector2& other) const {
		return { x - other.x, y - other.y };
	}

	Vector2 operator-() const {
		return Vector2(-x, -y);
	}

	Vector2 operator*(const Vector2& other)const {
		return Vector2(x * other.x, y * other.y);
	}

	Vector2 operator/(float scalar) const {
		return { x / scalar, y / scalar };
	}

	Vector2 operator/(const Vector2& other) const {
		return { x / other.x, y / other.y };
	}

	Vector2& operator=(const Vector2& other) {
		x = other.x; y = other.y; return *this;
	}
	Vector2& operator+=(const Vector2& other) {
		x += other.x; y += other.y; return *this;
	}
	Vector2& operator-=(const Vector2& other) {
		x -= other.x; y -= other.y;  return *this;
	}
	Vector2& operator*=(const Vector2& other) {
		x *= other.x; y *= other.y; return *this;
	}
	Vector2& operator/=(const Vector2& other) {
		x /= other.x; y /= other.y; return *this;
	}

	// 長さ
	float Length() const {
		return std::sqrt(x * x + y * y);
	}

	// 正規化
	Vector2 Normalization() const {
		float len = Length();
		if (len == 0) return Vector2(0, 0); // 0除算防止
		return Vector2(x / len, y / len);
	}

	// 内積
	float Dot(const Vector2& other) const {
		return x * other.x + y * other.y;
	}

	// 外積
	float Cross(const Vector2& other) const {
		return x * other.y - y * other.x;
	}

	// 距離
	static float Distance(const Vector2& v1, const Vector2& v2) {
		return (v1 - v2).Length();
	}



};

