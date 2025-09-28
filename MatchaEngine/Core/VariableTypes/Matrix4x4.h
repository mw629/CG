#pragma once

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